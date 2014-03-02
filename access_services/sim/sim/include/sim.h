/*                               -*- Mode: C -*-
 * Copyright (C) 2010, ST-Ericsson
 *
 * File name       : sim.h
 * Description     : Card Application Toolkit API and UICC API
 *
 * Author          : Jessica Nilsson <jessica.j.nilsson@stericsson.com>
 *
 */

#ifndef __sim_h__
#define __sim_h__ (1)

#include <sys/types.h>
#include <stdint.h>

/**
 * @brief Callback function used by the library.
 * @param cause	Describes why the function was called.
 * @param client_tag The client tag that was previously sent to one of the library functions.
 * @param data	Pointer to data.  Must be cast to a type depending on the cause.
 * @param user_data	User data as given in the closure.
 *
 * The callback is called by the library in the context of the reader thread
 * maintained by the library.
 * The callback is called when unsolicited requests are sent from the XXX server
 * or when replies to requests arrieve from the XXX server, or when the
 * connection to the XXX server is established or closed.
 */
typedef void            ste_sim_callback(int cause,
                                         uintptr_t client_tag,
                                         void *data,
                                         void *user_data);

/**
 * @brief Closure used by the library.
 */
typedef struct {
    ste_sim_callback       *func;       /**< Callback function related to the closure structure */
    void                   *user_data;  /**< Pointer to the user data associated with the closure */
} ste_sim_closure_t;


/**
 * @brief SIM API return values.
 * TODO:
 * At the moment, all error values are -1, to maintain compatibility with
 * the old code.  So testing for <0 or -1 will still work.
 * Client code should start using enum values.
 */
typedef enum ste_sim_error_tag {
    STE_SIM_SUCCESS,
    STE_SIM_ERROR_UNKNOWN = -1,          /**< Unspecified error (default if nothing better can be determined) */
    STE_SIM_ERROR_BAD_HANDLE = -2,       /**< The ste_sim_t handle was NULL, or somehow broken */
    STE_SIM_ERROR_PIPE = -3,             /**< An error relating to use of pipes */
    STE_SIM_ERROR_SOCKET = -4,           /**< An error relating to use of sockets */
    STE_SIM_ERROR_EVENT_STREAM = -5,     /**< An error relating to event streams */
    STE_SIM_ERROR_NOT_CONNECTED = -6,    /**< Not connected */
    STE_SIM_ERROR_RESOURCE = -7,         /**< Unable to allocate a resource - typically memory or threads */
} ste_sim_error_t;

/**
 * @brief Value for closed file descriptor.
 */
#define CLOSED_FD           -1

/**
 * @brief Cause for making a call back
 */
typedef enum {
    STE_SIM_CAUSE_NOOP,                                     /**< No operation. */
    STE_SIM_CAUSE_CONNECT,                                  /**< The connection is up and running. */
    STE_SIM_CAUSE_DISCONNECT,                               /**< The connection is down. */
    STE_SIM_CAUSE_SHUTDOWN,                                 /**< The client is registered. */
    STE_SIM_CAUSE_PING,                                     /**< The client is sending a PING. */
    STE_SIM_CAUSE_HANGUP,                                   /**< The connection has been terminated unexpectedly. */
    STE_SIM_CAUSE_STARTUP,                                  /**< The startup is complete */
    STE_SIM_CAUSE_SIM_LOG_LEVEL,                            /**< Set the debug level in simd */
    STE_SIM_CAUSE_MODEM_LOG_LEVEL,                          /**< Set the debug level in modem abstraction layer */
    STE_SIM_CAUSE__MAX,                                     /**< Base value for the next set of codes */
} ste_sim_cause_t;

/**
 * @brief Cause for making cat related call backs
 */
typedef enum {
    STE_CAT_CAUSE_NOOP = STE_SIM_CAUSE__MAX,                /**< No operation. */
    STE_CAT_CAUSE_REGISTER,                                 /**< The client is registered. */
    STE_CAT_CAUSE_PC,                                       /**< Proactive command received. */
    STE_CAT_CAUSE_SIM_EC_CALL_CONTROL,                      /**< The EC for call control. */
    STE_CAT_CAUSE_IND_CAT_STATUS,                           /**< The status of CAT has changed. */
    STE_CAT_CAUSE_EC,                                       /**< Envelope command. */
    STE_CAT_CAUSE_TR,                                       /**< Terminal response. */
    STE_CAT_CAUSE_SET_TERMINAL_PROFILE,                     /**< Set the terminal profile. */
    STE_CAT_CAUSE_GET_TERMINAL_PROFILE,                     /**< Get the terminal profile. */
    STE_CAT_CAUSE_GET_CAT_STATUS,                           /**< Get the status of the CAT. */
    STE_CAT_CAUSE_PC_NOTIFICATION,                          /**< Proactive Command Notification. */
    STE_CAT_CAUSE_DEREGISTER,                               /**< The client wishes to deregister. */
    STE_CAT_CAUSE_PC_SETUP_CALL_IND,                        /**< Proactive command to setup a call. */
    STE_CAT_CAUSE_ANSWER_CALL,                              /**< Answer a call. */
    STE_CAT_CAUSE_EVENT_DOWNLOAD,                           /**< Event Download. */
    STE_CAT_CAUSE_SIM_EC_SMS_CONTROL,                       /**< The EC for SMS control. */
    STE_CAT_CAUSE_SETUP_CALL_RESULT,                        /**< The setup call result. */
    STE_CAT_CAUSE_PC_REFRESH_IND,                           /**< Refresh Proactive command */
    STE_CAT_CAUSE_PC_REFRESH_FILE_IND,                      /**< File change by PC Refresh command */
    STE_CAT_CAUSE_SESSION_END_IND,                          /**< The PC session has ended */
    STE_CAT_CAUSE_ENABLE,                                   /**< CAT Enable */
    STE_CAT_CAUSE_CAT_INFO,                                 /**< CAT info ind */
    STE_CAT_CAUSE_RAT_SETTING_UPDATED,                      /**< SIM preferred RAT setting has been updated */
    STE_CAT_CAUSE__MAX,                                     /**< Base value for the next set of codes */
} ste_cat_cause_t;

/**
 * @brief Cause for making uicc related call backs
 *
 * Note!
 * Whenever this enum is updated also the ste_sim_cause_name string tables in tuicc.c and tcat.c have to be updated.
 */
typedef enum {
    STE_UICC_CAUSE_NOOP = STE_CAT_CAUSE__MAX,               /**< No operation. */
    STE_UICC_CAUSE_REGISTER,                                /**< The client is registered. */
    STE_UICC_CAUSE_REQ_PIN_VERIFY,                          /**< Pin verify request. */
    STE_UICC_CAUSE_REQ_PIN_NEEDED,                          /**< Pin needed indication received. */
    STE_UICC_CAUSE_REQ_READ_SIM_FILE_RECORD,                /**< Record-based EF read. */
    STE_UICC_CAUSE_REQ_UPDATE_SIM_FILE_RECORD,              /**< Record-based EF updated. */
    STE_UICC_CAUSE_REQ_PIN_CHANGE,                          /**< PIN change response received */
    STE_UICC_CAUSE_REQ_READ_SIM_FILE_BINARY,                /**< Transparent EF read */
    STE_UICC_CAUSE_REQ_UPDATE_SIM_FILE_BINARY,              /**< Transparent EF update */
    STE_UICC_CAUSE_REQ_SIM_FILE_GET_FORMAT,                 /**< Get file format response received. */
    STE_UICC_CAUSE_REQ_GET_FILE_INFORMATION,                /**< Get file information response received. */
    STE_UICC_CAUSE_REQ_GET_SIM_STATE,                       /**< Get sim state response received. */
    STE_UICC_CAUSE_SIM_STATE_CHANGED,                       /**< SIM state changed indication received. */
    STE_UICC_CAUSE_REQ_PIN_DISABLE,                         /**< PIN disable response received. */
    STE_UICC_CAUSE_REQ_PIN_ENABLE,                          /**< PIN enable response received. */
    STE_UICC_CAUSE_REQ_PIN_INFO,                            /**< PIN info response received. */
    STE_UICC_CAUSE_REQ_PIN_UNBLOCK,                         /**< PIN unblock response received. */
    STE_UICC_CAUSE_NOT_READY,                               /**< SIM not ready response received. */
    STE_UICC_CAUSE_REQ_GET_APP_INFO,                        /**< SIM Application Information received. */
    STE_UICC_CAUSE_REQ_READ_SMSC,                           /**< SMSC File Read. */
    STE_UICC_CAUSE_REQ_UPDATE_SMSC,                         /**< SMSC File updated. */
    STE_UICC_CAUSE_REQ_SIM_FILE_READ_GENERIC,               /**< Generic SIM File read. */
    STE_UICC_CAUSE_REQ_SIM_FILE_READ_IMSI,                  /**< IMSI File read. */
    STE_UICC_CAUSE_REQ_UPDATE_SERVICE_TABLE,                /**< Update the service table - enable/disable service */
    STE_UICC_CAUSE_REQ_GET_SERVICE_TABLE,                   /**< Read the enable service table - check if service is enabled/disabled */
    STE_UICC_CAUSE_SIM_STATUS,                              /**< SIM Status */
    STE_UICC_CAUSE_REQ_SIM_ICON_READ,                       /**< Read ICON file */
    STE_UICC_CAUSE_REQ_READ_SUBSCRIBER_NUMBER,              /**< Read Subscriber number */
    STE_UICC_CAUSE_REQ_SIM_FILE_UPDATE_PLMN,                /**< PLMN File updated. */
    STE_UICC_CAUSE_REQ_SIM_FILE_READ_PLMN,                  /**< PLMN File read. */
    STE_UICC_CAUSE_REQ_CARD_STATUS,                         /**< SIM card status */
    STE_UICC_CAUSE_REQ_APP_STATUS,                          /**< SIM application status */
    STE_UICC_CAUSE_REQ_APPL_APDU_SEND,                      /**< Send APDU to the active application */
    STE_UICC_CAUSE_REQ_SMSC_GET_ACTIVE,                     /**< SMSC get. */
    STE_UICC_CAUSE_REQ_SMSC_SET_ACTIVE,                     /**< SMSC set. */
    STE_UICC_CAUSE_REQ_SMSC_GET_RECORD_MAX,                 /**< SMSC record max. */
    STE_UICC_CAUSE_REQ_SMSC_SAVE_TO_RECORD,                 /**< SMSC save record. */
    STE_UICC_CAUSE_REQ_SMSC_RESTORE_FROM_RECORD,            /**< SMSC record restore. */
    STE_UICC_CAUSE_REQ_SIM_CHANNEL_SEND,                    /**< Send APDU to Application channel */
    STE_UICC_CAUSE_REQ_SIM_CHANNEL_OPEN,                    /**< Open an Application Channel */
    STE_UICC_CAUSE_REQ_SIM_CHANNEL_CLOSE,                   /**< Close an Application Channel */
    STE_UICC_CAUSE_REQ_GET_SERVICE_AVAILABILITY,            /**< Read the service table - check if service is available and activated */
    STE_UICC_CAUSE_REQ_SIM_FILE_READ_FDN,                   /**< FDN File read. */
    STE_UICC_CAUSE_REQ_SIM_FILE_READ_ECC,                   /**< ECC file read */
    STE_UICC_CAUSE_REQ_SIM_RESET,                           /**< SIM reset */
    STE_UICC_CAUSE_REQ_SIM_POWER_ON,                        /**< SIM card Power ON */
    STE_UICC_CAUSE_REQ_SIM_POWER_OFF,                       /**< SIM card Power OFF */
    STE_UICC_CAUSE_REQ_SIM_READ_PREFERRED_RAT_SETTING,      /**< SIM preferred RAT setting */
    STE_UICC_CAUSE__MAX,                                    /**< Base value for the next set of codes */
} ste_uicc_cause_t;


/**
 * @brief Incomplete type representing the XXX server.
 */
typedef struct ste_sim_s ste_sim_t;


/**
 * @brief               Create a new representation of the cat or uicc server.
 * @param closure       Closure to use.
 * @return	        Pointer to a new object representing the cat or uicc server.
 *
 * When creating the sim object with this function, a reader thread will be started
 * in the library when connecting to the server and this thread will handle incomming data
 * from the server.
 */

ste_sim_t              *ste_sim_new(const ste_sim_closure_t * closure);

/**
 * @brief               Creates a single threaded representation of the cat or uicc server.
 * @param closure       Closure to use.
 * @return              Pointer to a new object representing the cat or uicc server.
 *
 * When creating the sim object with this function, no reader thread will be started
 * in the library when connecting to the server and the client will be responsible for doing
 * the select() and to call the ste_sim_read function when data is to be handled.
 */
ste_sim_t* ste_sim_new_st(const ste_sim_closure_t * closure);

/**
* @brief                Retrieves the underlying file descriptor.
* @param sim            Object to operate on.
* @return               File descriptor on success or -1 on failure.
*
* The sim object must have been obtained from ste_sim_new_st and must
* also been connected to the server before this function is called for the
* function to succeed. The file descriptor is set to O_NONBLOCK and can be
* used in an select() call by the client.
*/
int        ste_sim_fd(const ste_sim_t* sim);

/**
* @brief                Used by the client to read incomming data from the server.
* @param sim            Object to operate on.
* @return               Status of the operation. 0 on success, -1 on failure.
*
* The function is used for reading available data after the select() call has
* returned in the client context. Note that this function shall only be used
* if the client has connected to the server with an sim object created by
* ste_sim_new_st.
*/
int        ste_sim_read(ste_sim_t* sim);

/**
 * @brief	     Delete a uicc or cat object.
 * @param sim        Object to operate on.
 * @param client_tag The client tag that will be returned in the callback function.
 *
 * If the object is connected to the server, it is disconnected before deleted.
 */

void ste_sim_delete(ste_sim_t * sim,
                    uintptr_t client_tag);

/**
 * @brief	     Connect to the uicc or cat server.
 * @param sim        Object to operate on.
 * @param client_tag The client tag that will be returned in the callback function.
 * @return           Status of the operation.  0 on success, -1 on failure.
 *
 * The connection to the cat or the uicc server is established.
 * When the sim object has been obtained through ste_sim_new, a reader thread is
 * launched and the closure callback function will be called in the context
 * of the reader thread with the STE_CAT_CAUSE_CONNECT/STE_UICC_CAUSE_CONNECT cause and data is 0.
 *
 * When the sim object has been obtained through ste_sim_new_st, the closure callback
 * function will be called in the context of the caller thread with the STE_CAT_CAUSE_CONNECT/STE_UICC_CAUSE_CONNECT
 * cause and data is 0.
 */
int ste_sim_connect(ste_sim_t * sim,
                    uintptr_t client_tag);

/**
 * @brief	     Disconnect from the cat or the uicc server.
 * @param sim        Object to operate on.
 * @param client_tag The client tag that will be returned in the callback function.
 * @return           Status of the operation.  0 on success, -1 on failure.
 *
 * The reader thread is informed to disconnect from the cat or the uicc server.
 * The reader thread calls the closure callback function with the cause
 * STE_CAT_CAUSE_DISCONNECT/STE_UICC_CAUSE_DISCONNECT and data is 0, when it has disconnected.
 * After the return of the closure callback function, the reader
 * thread terminates.
 */
int ste_sim_disconnect(ste_sim_t * sim,
                       uintptr_t client_tag);

/**
 * @brief	     Send a PING to the cat server.
 * @param cat        Object to operate on.
 * @param client_tag The client tag that will be returned in the callback function.
 * @return           Status of the operation.  0 on success, -1 on failure.
 *
 * This is intended as a basic functional test of the communication between
 * the cat client and the cat server.  A PING message is sent to the cat server
 * which replies with a PING.  The closure callback function is called with the
 * cause STE_CAT_CAUSE_PING/STE_UICC_CAUSE_PING when the response from the server arrives.
 * N.B. The return status of this function reflects the status of sending the
 * PING message to the server only.
 */
int ste_sim_ping(ste_sim_t * cat,
                 uintptr_t client_tag);

/**
 * @brief            Send a PING to the cat server, synchronous call
 * @param sim        Object to operate on.
 * @param client_tag The client tag that will be returned in the callback function.
 * @return           Status of the operation.  0 on success, -1 on failure.
 *
 * This is intended as a basic functional test of the communication between
 * the cat client and the cat server.  A PING message is sent to the cat server
 * which replies with a PING.  The closure callback function is called with the
 * cause STE_CAT_CAUSE_PING/STE_UICC_CAUSE_PING when the response from the server arrives.
 * N.B. The return status of this function reflects the status of sending the
 * PING message to the server only.
 */

int ste_sim_ping_sync( ste_sim_t* sim, uintptr_t  client_tag );

/**
 * @brief	     Send a SHUTDOWN message to the cat or the uicc server.
 * @param cat        Object to operate on.
 * @param client_tag The client tag that will be returned in the callback function.
 * @return           Status of the operation.  0 on success, -1 on failure.
 *
 * This is intended as a basic functional test of the communication between
 * the cat client and the cat server.  The closure callback function is called with the
 * cause STE_UICC_CAUSE_SHUTDOWN, when the response from the
 * server arrives.
 * N.B. The return status of this function reflects the status of sending the
 * SHUTDOWN message to the server only.
 */
int ste_sim_shutdown(ste_sim_t * cat,
                     uintptr_t client_tag);

/**
 * @brief	     Send a STARTUP message to the cat or the uicc server.
 * @param cat        Object to operate on.
 * @param client_tag The client tag that will be returned in the callback function.
 * @return           Status of the operation.  0 on success, -1 on failure.
 *
 * This is intended as a basic functional test of the communication between
 * the cat client and the cat server.  The closure callback function is called with the
 * cause STE_UICC_CAUSE_STARTUP, when the response from the
 * server arrives. This mechanism is intended to handle the modem start-up and simd start-up
 * in a controlled way. The MCE module (which might be replaced in the future) calls this method
 * which triggers the simd to open up connection with the modem and start communication with it.
 */
int ste_sim_startup(ste_sim_t * cat,
                     uintptr_t client_tag);


/**
 * @brief               Set the debug level of the sim daemon
 * @param cat           Object to operate on.
 * @param client_tag    The client tag that will be returned in the callback function.
 * @param level         The new debug level
 * @return              Status of the operation.  0 on success, -1 on failure.
 */
int ste_sim_set_sim_log_level(ste_sim_t * cat,
                              uintptr_t client_tag,
                              int level);

/**
 * @brief               Set the debug level of the modem abstraction layer
 * @param cat           Object to operate on.
 * @param client_tag    The client tag that will be returned in the callback function.
 * @param level         The new debug level
 * @return              Status of the operation.  0 on success, -1 on failure.
 */
int ste_sim_set_modem_log_level(ste_sim_t * cat,
                                uintptr_t client_tag,
                                int level);


// Card Application Toolkit API

// Unsolicited events from CATD to interested clients
#define STE_CAT_CLIENT_REG_EVENTS_CAT_STATUS          0x00000001  // Set for CAT STATUS
#define STE_CAT_CLIENT_REG_EVENTS_CARD_STATUS         0x00000002  // Set for CARD STATUS
#define STE_CAT_CLIENT_REG_EVENTS_PC                  0x00000004  // Set for clients that implements/terminates proactive commands.
#define STE_CAT_CLIENT_REG_EVENTS_PC_NOTIFICATION     0x00000008  // Set for clients that handles the UI tasks for proactive commands that are implemented/terminated within CATD.
#define STE_CAT_CLIENT_REG_EVENTS_PC_REFRESH          0x00000010  // Set for clients that handle the REFRESH proactive command
#define STE_CAT_CLIENT_REG_EVENTS_PC_SETUP_CALL_IND   0x00000020  // Det for clients that handle the PC setup call indication
#define STE_CAT_CLIENT_REG_EVENTS_SESSION_END_IND     0x00000040  // Det for clients that handle the session end indication
#define STE_CAT_CLIENT_REG_EVENTS_GUI_CAPABLE_CLIENT  0x80000000  // Indicates that the client that handles UI parts has a graphical UI (i.e. can/will display icons).
#define STE_CAT_CLIENT_REG_EVENTS_CAT_INFO            0x00000080  // Set for CAT info indication
#define STE_CAT_CLIENT_REG_EVENTS_RAT_SETTING_UPDATED 0x00000100  // Indicates that the SIM mandated RAT setting preference has changed

//status word from catd
#define STE_CAT_RESPONSE_OK                         0x9000    /**< The status word from catd, used for cat response for TR, EC */
#define STE_CAT_RESPONSE_CARD_MEMORY_PROBLEM        0x9240    /**< The status word from catd, used for cat response for TR, EC */
#define STE_CAT_RESPONSE_CARD_BUSY                  0x9300    /**< The status word from catd, used for cat response for TR, EC */
#define STE_CAT_RESPONSE_INCORRECT_PARAMETER        0x6700    /**< The status word from catd, used for cat response for TR, EC */
#define STE_CAT_RESPONSE_NOT_SPECIFIED_ERROR        0x6F00    /**< The status word from catd, used for cat response for TR, EC */
#define STE_CAT_RESPONSE_UNKNOWN                    0xFFFF    /**< The status word from catd, used for cat response for TR, EC */
#define STE_CAT_RESPONSE_TEMP_PROBLEM_IND           0xFFFE    /**< The status word from catd, used for cat response for TR, EC */

//nmr
#define MAX_NMR_RES_SIZE 16
#define MAX_NMR_ARFCN_SIZE 65 /* 32 channels 2 byte for each */
#define MAX_NMR_REPORT_SIZE 250

/**
 * @brief Data structure containing the two status words as defined by 3GPP TS 51.011
 */
typedef struct {
  uint8_t sw1;  /**< sw1 Status word 1 */
  uint8_t sw2;  /**< sw2 Status word 2 */
} ste_uicc_status_word_t;

/**
 * @brief Data structure holding APDU data.
 *
 * This is used to pass an APDU into the callback function when the cause is
 * STE_CAT_CAUSE_PC.
 */
typedef struct {
    char                   *buf;        /**< Pointer to start of APDU data */
    unsigned                len;        /**< Length, in bytes, of APDU data */
} ste_cat_apdu_data_t;

/**
 * @brief General Data structure holding cat service status.
 *
 * This is used to pass status into the callback function when only status value is
 * passed back from catd.
 */
typedef struct {
    int                     status;        /**< Status of the CAT service */
} ste_cat_status_t;

typedef enum {
    STE_CAT_SERVICE_NULL      = 0x00,
    STE_CAT_SERVICE_INIT,
    STE_CAT_SERVICE_TPDL,
    STE_CAT_SERVICE_READY,
    STE_CAT_SERVICE_REFRESH,
    STE_CAT_SERVICE_MODEM_ERROR
} ste_cat_service_status_t;


/**
 * @brief General Data structure holding information about a Proactive command from SIMD.
 *
 * This is used to pass PC into the callback function when only PC or PC notification is
 * sent from catd.
 */
typedef struct {
    uintptr_t            simd_tag;    /**< The client tag generated by SIMD, needs to be passed back when respond with TR, can be NULL */
    ste_cat_apdu_data_t  apdu;        /**< original APDU for the PC */
} ste_cat_pc_ind_t;


/**
 * @brief Data structure holding cat service status.
 *
 * This is used to pass status into the callback function when the cause is
 * STE_CAT_CAUSE_IND_CAT_STATUS. The real status of CAT service is returned
 * in the status variable, the value of which should be mapped to ste_cat_service_status_t
 */
typedef ste_cat_status_t    ste_cat_cat_status_t;

/**
 * @brief Data structure holding cat envelope command response.
 *
 * This is used to pass status into the callback function when the cause is
 * STE_CAT_CAUSE_EC.
 */
typedef struct {
    uint16_t             ec_status;    /**< The status word from catd */
    ste_cat_apdu_data_t  apdu;         /**< APDU for the cat response */
} ste_cat_ec_response_t;

/**
 * @brief Data structure holding cat terminal response response.
 *
 * This is used to pass status into the callback function when the cause is
 * STE_CAT_CAUSE_TR.
 */
typedef struct {
    int                    status;      /**< The status of the operation. 0 == Success,
                                                                          1 == Request Failure,
                                                                          2 == Internal error */
    ste_uicc_status_word_t status_word; /**< Status words */
} ste_cat_tr_response_t;

/**
 * @brief Data structure holding set terminal profile response.
 *
 * This is used to pass status into the callback function when the cause is
 * STE_CAT_CAUSE_SET_TERMINAL_PROFILE.
 */
typedef ste_cat_status_t    ste_cat_set_terminal_profile_response_t;

/**
 * @brief Data structure holding get terminal profile response.
 *
 * This is used to pass status into the callback function when the cause is
 * STE_CAT_CAUSE_GET_TERMINAL_PROFILE.
 */
typedef struct {
    int                     status;        /**< Status of the CAT service */
    char                   *profile_p;     /**< fetched profile data */
    unsigned                profile_len;   /**< length of fetched profile data */
} ste_cat_get_terminal_profile_response_t;


/**
 * @brief	     Register with the cat server as a cat client.
 * @param cat        Object to operate on.
 * @param client_tag The client tag that will be returned in the callback function.
 * @param reg_events Events that the client is interested in, one bit represent one event.
 * @return           Status of the operation.  0 on success, -1 on failure.
 *
 * The client registers with the cat server that it will accept receiving
 * proactive commands.
 * The closure callback function is called with data set to 0 and the cause to
 * STE_CAT_CAUSE_REGISTER when the server acknowledges registration.
 * N.B. The return status of this function reflects the status of sending the
 * underlying message to the server only.
 */
int                     ste_cat_register(ste_sim_t * cat,
                                         uintptr_t client_tag,
                                         uint32_t reg_events);

/**
 * @brief	     DeRegister from the cat server.
 * @param cat        Object to operate on.
 * @param client_tag The client tag that will be returned in the callback function.
 * @return           Status of the operation.  0 on success, -1 on failure.
 *
 * The client deregisters from the cat server to tell it will not accept any
 * CAT indication/events.
 * The closure callback function us called with data set to 0 and the cause to
 * STE_CAT_CAUSE_DEREGISTER when the server acknowledges registration.
 * N.B. The return status of this function reflects the status of sending the
 * underlying message to the server only.
 */
int                     ste_cat_deregister(ste_sim_t * cat, uintptr_t client_tag);

/**
 * @brief	     Set the terminal profile.
 * @param cat        Object to operate on.
 * @param client_tag The client tag that will be returned in the callback function.
 * @param data_p     Pointer to buffer where the terminal profile string(APDU) is stored.
 * @param len        Length, in bytes, of the data in the buffer.
 * @return           Status of the operation.  0 on success, -1 on failure.
 *
 * The client registers with the cat server that it will accept receiving
 * proactive commands.
 * In the closure callback function, the cause is set to
 * STE_CAT_CAUSE_SET_TERMINAL_PROFILE when the server acknowledges the operation.
 * N.B. The return status of this function reflects the status of sending the
 * underlying message to the server only.
 */
int                     ste_cat_set_terminal_profile(ste_sim_t * cat,
                                                     uintptr_t client_tag,
                                                     const char *data_p,
                                                     size_t len);

/**
 * @brief	     Get the terminal profile.
 * @param cat        Object to operate on.
 * @param client_tag The client tag that will be returned in the callback function.
 * @return           Status of the operation.  0 on success, -1 on failure.
 *
 * The client registers with the cat server that it will accept receiving
 * proactive commands.
 * In the closure callback function, the cause is set to
 * STE_CAT_CAUSE_GET_TERMINAL_PROFILE when the server acknowledges the operation.
 * N.B. The return status of this function reflects the status of sending the
 * underlying message to the server only.
 */
int                     ste_cat_get_terminal_profile(ste_sim_t * cat,
                                                     uintptr_t client_tag);

/**
 * @brief	     Get the CAT service status.
 * @param cat        Object to operate on.
 * @param client_tag The client tag that will be returned in the callback function.
 * @return           Status of the operation.  0 on success, -1 on failure.
 *
 * The client registers with the cat server that it will accept receiving
 * proactive commands.
 * In the closure callback function, the cause is set to
 * STE_CAT_CAUSE_GET_CAT_STATUS when the server acknowledges the operation.
 * N.B. The return status of this function reflects the status of sending the
 * underlying message to the server only. The real status of CAT service is returned
 * in the status variable, the value of which should be mapped to ste_cat_service_status_t
 */
int                     ste_cat_get_cat_status(ste_sim_t * cat,
                                               uintptr_t client_tag);

/**
 * @brief	     Send a terminal response to the server.
 * @param cat        Object to operate on.
 * @param client_tag The client tag that will be returned in the callback function.
 * @param buf        Pointer to buffer where the APDU is stored.
 * @param len        Length, in bytes, of the data in the buffer.
 * @return           Status of the operation.  0 on success, -1 on failure.
 *
 * A terminal response message is sent to the server.
 * N.B. The return status of this function reflects the status of sending the
 * underlying message to the server only.
 *
 * This should be used after a proactive command has been received from the
 * server.  When a proactive command is received from the server, the closure
 * callback function is called with cause STE_CAT_IND_PC and data points to
 * a structure containing a buffer holding the APDU.
 */
int                     ste_cat_terminal_response(ste_sim_t * cat,
                                                  uintptr_t client_tag,
                                                  const char *buf,
                                                  unsigned len);

/**
 * @brief	     Send an envelope command to the server.
 * @param cat        Object to operate on.
 * @param client_tag The client tag that will be returned in the callback function.
 * @param buf        Pointer to buffer where the APDU is stored.
 * @param len        Length, in bytes, of the data in the buffer.
 * @return           Status of the operation.  0 on success, -1 on failure.
 *
 * An envelope command is sent to the server.
 * N.B. The return status of this function reflects the status of sending the
 * underlying message to the server only.
 */
int                     ste_cat_envelope_command(ste_sim_t * cat,
                                                uintptr_t client_tag,
                                                const char *buf,
                                                unsigned len);



typedef enum
{
    STE_SIM_TON_UNKNOWN              = 0x00,
    STE_SIM_TON_INTERNATIONAL        = 0x01,
    STE_SIM_TON_NATIONAL             = 0x02,
    STE_SIM_TON_NETWORK_SPECIFIC     = 0x03,
    STE_SIM_TON_DEDICATED_PAD_ACCESS = 0x04,
    STE_SIM_TON_ALPHANUMERIC         = 0x05,
    STE_SIM_TON_ABBREVIATED          = 0x06,
    STE_SIM_TON_RESERVED_EXT         = 0x07,
    STE_SIM_TON_NOT_A_DIALLING_NUMBER = 0x07 // 3GPP TS 31.102 states that 0xFF should be set when SSC String does not contain a dialling number.
} ste_sim_type_of_number_t;

typedef enum
{
    STE_SIM_NPI_UNKNOWN       = 0x00,
    STE_SIM_NPI_ISDN          = 0x01,
    STE_SIM_NPI_DATA          = 0x03,
    STE_SIM_NPI_TELEX         = 0x04,
    STE_SIM_NPI_SC_SPECIFIC_1 = 0x05,
    STE_SIM_NPI_SC_SPECIFIC_2 = 0x06,
    STE_SIM_NPI_NATIONAL      = 0x08,
    STE_SIM_NPI_PRIVATE       = 0x09,
    STE_SIM_NPI_ERMES         = 0x0A,
    STE_SIM_NPI_RESERVED_EXT  = 0x0F,
    STE_SIM_NPI_NOT_A_DIALLING_NUMBER = 0x0F // 3GPP TS 31.102 states that 0xFF should be set when SSC String does not contain a dialling number.
} ste_sim_numbering_plan_id_t;

typedef enum
{
    STE_CAT_CC_TYPE_UNKNOWN,
    STE_CAT_CC_CALL_SETUP,       //CALL SETUP
    STE_CAT_CC_SS,               //SS
    STE_CAT_CC_USSD,             //USSD
    STE_CAT_CC_PDP               //PDP context activation
} ste_cat_cc_dialled_string_type_t;

/**
 * The available text coding formats.
 *
 * @param STE_SIM_CODING_UNKNOWN      Coding is undefined.
 * @param GSM_Default                 3GPP TS 23.038 GSM 7-bit including extension.
 * @param UCS2                        UCS-2 Unicode.
 * @param UCS2_81Prefix               3GPP TS 11.11 Annex B form 2.
 * @param UCS2_82Prefix               3GPP TS 11.11 Annex B form 3.
 * @param ASCII                       7-bit US ASCII.
 * @param ISO8859_1                   8-bit ISO 8859-1 (Latin 1)
 * @param UTF7                        UTF-7 Unicode.
 * @param UTF8                        UTF-8 Unicode.
 * @param USSD_PackedGSM_Default      3GPP TS 23.038 USSD packing.
 * @param UTF16                       UTF-16 Unicode.
 * @param SJIS                        Multi-byte Japanese JIS X0208/X0201 .
 * @param ISO2022                     Multi-byte CJK with ISO 2022 escape sequences.
 * @param EUC                         Extended Unix Code, Japanese only.
 * @param WIN1251                     Microsoft Windows Code Page for Cyrillic.
 * @param WIN1252                     Microsoft Windows Code Page for Latin 1.
 * @param KOI8R                       A popular Cyrillic code page for Russian.
 * @param ISO8859_2                   8-bit ISO 8859-2 (1987 Latin 2).
 * @param ISO8859_5                   8-bit ISO 8859-5 (1988 Cyrillic).
 * @param ISO8859_10                  8-bit ISO 8859-2 (1993 Latin 6).
 * @param GB2312                      GB 2312-80 for simplified Chinese.
 * @param GB18030                     GB 18030-2000 16-bit characters only.
 * @param BigFive                     Big 5 traditional Chinese.
 * @param BCD                         GSM 11.11 Binary Coded Decimal.
 * @param STE_SIM_INVALID_TEXT_CODING Coding is undefined.
 *
 * @note
 * ASCII7 and ASCII8 are depreciated names.
 * define ASCII8            ISO8859_1
 * define ASCII7            ASCII
 */

typedef enum
{
    STE_SIM_CODING_UNKNOWN,
//The following 3 types of coding scheme are supported by SIM/SAT
    STE_SIM_UCS2,
    STE_SIM_ASCII8,  //Specially for ISO8859_1 only
    STE_SIM_BCD,

//The types of coding scheme below are NOT supported by SIM/SAT,
//SIM/SAT only provide the coding type, clients need to decode/encode those strings by themselves
    STE_SIM_GSM_Default,
    STE_SIM_UCS2_81Prefix,
    STE_SIM_UCS2_82Prefix,
    STE_SIM_ASCII7,
    STE_SIM_UTF7,
    STE_SIM_UTF8,
    STE_SIM_USSD_PackedGSM_Default,
    STE_SIM_UTF16,
    STE_SIM_SJIS,
    STE_SIM_ISO2022,
    STE_SIM_EUC,
    STE_SIM_WIN1251,
    STE_SIM_WIN1252,
    STE_SIM_KOI8R,
    STE_SIM_ISO8859_2,
    STE_SIM_ISO8859_5,
    STE_SIM_ISO8859_10,
    STE_SIM_GB2312,
    STE_SIM_GB18030,
    STE_SIM_BigFive,

    // The one below is considered as well.
    STE_SIM_INVALID_TEXT_CODING
} ste_sim_text_coding_t;

/**
 * @brief Sim text struct with parameters: text coding, number of chars and actual text.
 */

typedef struct
{
    ste_sim_text_coding_t     text_coding;      /**< coding scheme for the text */
    unsigned                  no_of_characters; /**< The number of CHARACTERS. NOT bytes!!! */
    void*                     text_p;           /**< the text string */
} ste_sim_text_t;

/**
 * @brief Sim string struct with parameters: text coding, number of bytes and actual string.
 */

typedef struct
{
    ste_sim_text_coding_t     text_coding;      /**< coding scheme for the text */
    unsigned                  no_of_bytes;      /**< The number of bytes, NOT CHARACTERS!!! */
    uint8_t*                  str_p;           /**< the text string */
} ste_sim_string_t;

/**
 * @brief Dialled address struct with parameters ton, npi and actual dialled string/number.
 */

typedef struct
{
    ste_sim_type_of_number_t           ton;     /**< ton info for dialled address */
    ste_sim_numbering_plan_id_t        npi;     /**< npi info for dialled address */
    ste_sim_text_t                    *dialled_string_p;     /**< dialled string for call control */
} ste_cat_cc_dialled_address_t;

/**
 * @brief USSD data struct with parameters: DCS and actual USSD string.
 */

typedef struct
{
    uint8_t                            dcs;                 /**< dcs for ussd */
    ste_sim_string_t                  *dialled_string_p;    /**< dialled string for ussd */
} ste_cat_cc_ussd_data_t;

/** Maximum size in bytes of sub-address */
#define STE_CAT_CC_MAX_SIZE_SUB_ADDRESS            (80)
/** Maximum size in bytes of Capability Configuration Parameters */
#define STE_CAT_CC_MAX_SIZE_CCP                    (14)

/**
 * @brief sub_address struct parameters: length and actual sub_address.
 */

typedef struct
{
    uint8_t    len;             /**< length of sub address */
    uint8_t   *sub_address_p;   /**< sub address data */
} ste_cat_cc_sub_address_t;

/**
 * @brief Call control CCP struct, paramters: Length of ccp and actual ccp.
 */

typedef struct
{
    uint8_t    len;     /**< length of ccp */
    uint8_t   *ccp_p;   /**< ccp data */
} ste_cat_cc_ccp_t;

/**
 * @brief Call control call setup struct, paramters: address, sub_address, ccp 1, 2 and BC repeat indication.
 */

typedef struct
{
    ste_cat_cc_dialled_address_t       address;         /**< dialled address */
    ste_cat_cc_sub_address_t           sub_address;     /**< sub address */
    ste_cat_cc_ccp_t                   ccp_1;           /**< capability configuration param 1 */
    ste_cat_cc_ccp_t                   ccp_2;           /**< capability configuration param 1 */
    uint8_t                            bc_repeat_ind;   /**< BC repeat indication */
} ste_cat_cc_call_setup_t;

/**
 * @brief CAll control SS, supplementary services for Call control struct parameter: dialled address.
 */

typedef struct
{
    ste_cat_cc_dialled_address_t       address;  /**< SS CONTROL data */
} ste_cat_cc_ss_t;

/**
 * @brief CAll control USSD, unstructured supplementary service data for Call control struct parameter: dialled address.
 */

typedef struct
{
    ste_cat_cc_ussd_data_t             ussd_data;  /**< USSD CONTROL data */
} ste_cat_cc_ussd_t;

typedef struct
{
    ste_sim_string_t                   pdp_context;  /**< PDP context activation request string */
} ste_cat_cc_pdp_t;


//the union structure for different types of call control
typedef union
{
    ste_cat_cc_call_setup_t         *call_setup_p;           //call setup control
    ste_cat_cc_ss_t                 *ss_p;                   //SS
    ste_cat_cc_ussd_t               *ussd_p;                 //USSD
    ste_cat_cc_pdp_t                *pdp_p;                  //PDP
} ste_cat_cc_data_t;

/**
 * @brief Data structure holding call control data.
 *
 * This is used to pass control information into the card
 */
typedef struct
{
    ste_cat_cc_dialled_string_type_t   cc_type;  /**< The type of CC. */
    ste_cat_cc_data_t                  cc_data;  /**< The real data structure. */
} ste_cat_call_control_t;

/**
 * @brief	Request the SIM to perform call control on a dialled address, SS string or USSD string.
 * @param cat   Object to operate on.
 * @param client_tag The client tag that will be returned in the callback function.
 * @param cc_p pointer to the call control object.
 *
 * @return      Status of the operation.  0 on success, -1 on failure.
 */
int                     ste_cat_call_control(ste_sim_t              * cat,
                                             uintptr_t                client_tag,
                                             ste_cat_call_control_t * cc_p);

/** PC setup call details */
typedef enum {
    STE_CAT_ONLY_IF_NOT_BUSY,
    STE_CAT_PUT_ALL_OTHER_CALLS_ON_HOLD,
    STE_CAT_DISCONNECT_ALL_OTHER_CALLS
} ste_cat_setup_call_option_t;

/**
 * @brief Data structure holding PC setup call indication.
 *
 * This is used to pass pc setup call event into the callback function when the cause is
 * STE_CAT_CAUSE_PC_SETUP_CALL_IND.
 */
typedef struct {
    uintptr_t                     simd_tag;    /**< The client tag generated by SIMD, can be NULL */
    unsigned                      duration;    /**< timeout value for the call */
    int                           redial;      /**< redial is needed or not, (0 == redial is not needed) */
    ste_cat_setup_call_option_t   option;      /**< call setup conflict options */
} ste_cat_pc_setup_call_ind_t;

/**
 * @brief Calling number for setup call struct including coding, actual number, ton, npi and number of chars.
 */

typedef struct {
    ste_sim_text_coding_t         text_coding;      /**< coding scheme for the text */
    unsigned                      no_of_characters; /**< The number of CHARACTERS */
    uint8_t*                      text_p;           /**< the text string */
    ste_sim_type_of_number_t      ton;              /**< ton info for dialled address */
    ste_sim_numbering_plan_id_t   npi;              /**< npi info for dialled address */
} ste_cat_setup_call_number_t;

/**
 * @brief Setup call details struct with params address, sub_address, ccp, duration, if redial needed,
 * if cc needed and conflict options
 */

typedef struct {
    int                           cc_needed;   /**< call control is needed or not, (0 == redial is not needed) */
    int                           redial;      /**< redial is needed or not, (0 == redial is not needed) */
    unsigned                      duration;    /**< timeout value for the call */
    ste_cat_setup_call_option_t   option;      /**< call setup conflict options */
    ste_cat_setup_call_number_t   address;     /**< dialled address */
    ste_cat_cc_sub_address_t      sub_address; /**< sub address */
    ste_cat_cc_ccp_t              ccp;         /**< capability configuration param */
} ste_cat_setup_call_details_t;

typedef enum {
    STE_CAT_SETUP_CALL_OK = 0,
    STE_CAT_SETUP_CALL_ME_UNABLE_TO_PROCESS,
    STE_CAT_SETUP_CALL_ME_BUSY_ON_CALL,
    STE_CAT_SETUP_CALL_ME_BUSY_ON_SS,
    STE_CAT_SETUP_CALL_ME_BUSY_ON_USSD,
    STE_CAT_SETUP_CALL_ME_BUSY_ON_DTMF,
    STE_CAT_SETUP_CALL_NW_UNABLE_TO_PROCESS,
    STE_CAT_SETUP_CALL_REMOTE_REJECTED,
    STE_CAT_SETUP_CALL_REMOTE_UNAVAILABLE,
    STE_CAT_SETUP_CALL_DURATION_TIMEOUT
} ste_cat_setup_call_result_t;

/**
 * @brief Data structure holding answer call response.
 *
 * This is used to pass setup call status into the callback function when the cause is
 * STE_CAT_RSP_ANSWER_CALL.
 */
typedef ste_cat_status_t    ste_cat_answer_call_response_t;


/**
 * @brief	Request the SIM to setup/reject the call iniated by PC setup call from SIM.
 * @param cat   Object to operate on.
 * @param client_tag The client tag that will be returned in the callback function.
 * @param answer     decision about answer the call or not (1 == answer, 0 == reject).
 * @return      Status of the operation.  0 on success, -1 on failure.
 *
 */
int                     ste_cat_answer_call(ste_sim_t   * cat,
                                            uintptr_t     client_tag,
                                            int           answer);

/**
 * @brief Enum values for event download type.
 *
 * This is used to pass event type into catd when specific event happens
 * Note: the enum value of all members should not be changed
 *       They are mapping with specification
 */
typedef enum
{
    STE_CAT_EVENT_DOWNLOAD_TYPE_MT_CALL                      = 0,
    STE_CAT_EVENT_DOWNLOAD_TYPE_CALL_CONNECTED               = 1,
    STE_CAT_EVENT_DOWNLOAD_TYPE_CALL_DISCONNECTED            = 2,
    STE_CAT_EVENT_DOWNLOAD_TYPE_LOCATION_STATUS              = 3,
    STE_CAT_EVENT_DOWNLOAD_TYPE_USER_ACTIVITY                = 4,
    STE_CAT_EVENT_DOWNLOAD_TYPE_IDLE_SCREEN_AVAILABLE        = 5,
    STE_CAT_EVENT_DOWNLOAD_TYPE_CARD_READER_STATUS           = 6,
    STE_CAT_EVENT_DOWNLOAD_TYPE_LANGUAGE_SELECTION           = 7,
    STE_CAT_EVENT_DOWNLOAD_TYPE_BROWSER_TERMINATED           = 8,
    STE_CAT_EVENT_DOWNLOAD_TYPE_DATA_AVAILABLE               = 9,
    STE_CAT_EVENT_DOWNLOAD_TYPE_CHANNEL_STATUS               = 10,
    STE_CAT_EVENT_DOWNLOAD_TYPE_ACCESS_TECHNOLOGY_CHANGE     = 11,
    STE_CAT_EVENT_DOWNLOAD_TYPE_DISPLAY_PARAMETERS_CHANGED   = 12,
    STE_CAT_EVENT_DOWNLOAD_TYPE_LOCAL_CONNECTION             = 13,
    STE_CAT_EVENT_DOWNLOAD_TYPE_NW_SEARCH_MODE_CHANGE        = 14,
    STE_CAT_EVENT_DOWNLOAD_TYPE_BROWSING_STATUS              = 15,
    STE_CAT_EVENT_DOWNLOAD_TYPE_FRAMES_INFO_CHANGE           = 16,
    STE_CAT_EVENT_DOWNLOAD_TYPE_HCI_CONNECTIVITY_EVENT       = 19,
    STE_CAT_EVENT_DOWNLOAD_TYPE_END                          = 20
} ste_cat_event_download_type_t;

/**
 * @brief Event struct for MT call with parameters: transaction id, address and sub_address.
 */

typedef struct
{
    uint8_t                      transaction_id;  /**< Transaction id of the call from network. */
    ste_cat_setup_call_number_t  address;         /**< Address */
    ste_cat_cc_sub_address_t     sub_address;     /**< Sub address */
} ste_cat_event_mt_call_t;

/**
 * @brief Event struct call connected with parameter: transaction id.
 */

typedef struct
{
    uint8_t                      transaction_id;  /**< Transaction id of the call from network. */
} ste_cat_event_call_connected_t;

/**
 * @brief Event struct for call disconnected, parameters: cause and transaction id.
 */

typedef struct
{
    uint8_t                      transaction_id;  /**< Transaction id of the call from network. */
    uint8_t                      cause_len;       /**< The length of the cause. */
    uint8_t                    * cause_p;         /**< The cause of the disconnection. */
} ste_cat_event_call_disconnected_t;

/**
 * @brief Measurement results and BCCH channels applicable if rat is MAL_NET_NMR_RAT_GERAN
 */
typedef struct
{
    uint8_t   measurement_results[MAX_NMR_RES_SIZE];  /**< Measurement Results.
                                                      * The contents are equal to Measurements Results
                                                      * information element starting at octet 2. The
                                                      * description can be found in GSM TS 24.008.
                                                      */
    uint8_t   num_of_channels;                        /**< Number of BCCH channels: 0 to 32 */
    uint16_t  ARFCN_list[MAX_NMR_ARFCN_SIZE];         /**< An array uint16_t[num_of_channels] */
} ste_cat_nmr_geran_t;

/**
 * @brief Measurement report struct applicable if rat is MAL_NET_NMR_RAT_UTRAN.
 */

typedef struct
{
    uint8_t   mm_report_len;  /**< Length of Measurement Report */
    uint8_t   measurements_report[MAX_NMR_REPORT_SIZE];  /**< Measurement Report.
                                                         * The contents are equal to Measurements Report
                                                         * coded as RRC:UL-DCCH message. The description
                                                         * can be found in 3GPP TS 25.331.
                                                         */
} ste_cat_nmr_utran_t;

/**
 * @brief Event struct with 1 byte card reader status.
 */

typedef struct
{
    uint8_t                      card_status;  /**< Card reader status, 1 byte. */
} ste_cat_event_card_reader_status_t;

/**
 * @brief Event struct for Language selection with parameters: char 1 and 2.
 */

typedef struct
{
    uint8_t                      char_1;  /**< Language code char 1. */
    uint8_t                      char_2;  /**< Language code char 2. */
} ste_cat_event_language_selection_t;

/**
 * @brief Event struct for browser termination, parameter: cause.
 */

typedef struct
{
    uint8_t                      cause;  /**< 1 byte for cause. */
} ste_cat_event_browser_termination_t;

/**
 * @brief Event struct for data available with parameters: channel status 1,2 and data length.
 */

typedef struct
{
    uint8_t                      channel_status_1;  /**< Channel status byte 1. */
    uint8_t                      channel_status_2;  /**< Channel status byte 2. */
    uint8_t                      channel_data_len;  /**< Channel data length, 1 byte. */
} ste_cat_event_data_available_t;

/**
 * @brief Event struct for channel status with paramters: channel status 1,2, bearer type, length,
 * acutal bearer, address type, address length and actual address.
 */

typedef struct
{
    uint8_t                      channel_status_1;  /**< Channel status byte 1. */
    uint8_t                      channel_status_2;  /**< Channel status byte 2. */
    uint8_t                      bearer_type;       /**< Bearer type. */
    uint8_t                      bearer_len;        /**< Bearer len. */
    uint8_t                    * bearer_p;          /**< Bearer value. */
    uint8_t                      address_type;      /**< Address type. */
    uint8_t                      address_len;       /**< Address len. */
    uint8_t                    * address_p;         /**< Address value. */
} ste_cat_event_channel_status_t;

/**
 * @brief Event struct for Access Technology change with parameter: actual access technolgy.
 */

typedef struct
{
    uint8_t                      access_tech;  /**< 1 byte for access technology */
} ste_cat_event_access_technology_change_t;

/**
 * @brief event struct with display param changes, params 1,2 and 3.
 */

typedef struct
{
    uint8_t                      param_1;  /**< Param 1 */
    uint8_t                      param_2;  /**< Param 2 */
    uint8_t                      param_3;  /**< Param 3 */
} ste_cat_event_display_param_change_t;

/**
 * @brief Data structure holding event download response.
 *
 * This is used to pass event download status into the callback function when the cause is
 * STE_CAT_RSP_EVENT_DOWNLOAD.
 */

typedef ste_cat_status_t    ste_cat_event_download_response_t;

/**
 * @brief	Request CAT to perform event download when specific event happens.
 * @param cat   Object to operate on.
 * @param client_tag The client tag that will be returned in the callback function.
 * @param event_type The event type to be download to SIM.
 * @param event_data_p  The data related to the event type, one of the strutures defined above, or NULL
 * @return      Status of the operation.  0 on success, -1 on failure.
 *
 */

int                     ste_cat_event_download(ste_sim_t                     * cat,
                                               uintptr_t                       client_tag,
                                               ste_cat_event_download_type_t   event_type,
                                               void                          * event_data_p);

/**
 * @brief Data structure holding cat enable response.
 *
 * This is used to pass cat enable status into the callback function when the cause is
 * STE_CAT_CAUSE_ENABLE.
 */

typedef ste_cat_status_t    ste_cat_enable_response_t;

/**
 * @brief Enum values for cat enable type.
 *
 * This is used to pass cat enable type into catd
 */

typedef enum
{
    STE_CAT_ENABLE_TYPE_ENABLE  = 0,
    STE_CAT_ENABLE_TYPE_DISABLE = 1,
    STE_CAT_ENABLE_TYPE_NONE    = 2
} ste_cat_enable_type_t;

/**
 * @brief   Request CAT to perform cat enable when specific event happens.
 * @param cat   Object to operate on.
 * @param client_tag The client tag that will be returned in the callback function.
 * @param enable_type The enable type .
 * @return      Status of the operation.  0 on success, -1 on failure.
 *
 */

int                     ste_cat_enable(ste_sim_t             * cat,
                                       uintptr_t               client_tag,
                                       ste_cat_enable_type_t   enable_type);

/**
 * @brief Enum values for cat info.
 *
 * This is used to pass cat status info back to clients
 */

typedef enum
{
     STE_CAT_INFO_SUCCESS      = 0x00,
     STE_CAT_INFO_NOT_SUPPORTED,
     STE_CAT_INFO_PROFILE_FAIL,
     STE_CAT_INFO_NULL
} ste_cat_info_t;

/**
  * @brief General Data structure holding cat info.
  *
  * This is used to pass status into the callback function when only status value is
  * passed back from catd.
  */

typedef struct {
    ste_cat_info_t     info;        /**< Status of the CAT startup */
} ste_cat_info_ind_t;


/**
 * @brief Data structure holding sms control data.
 *
 * This is used to pass control information into the card
 */

typedef struct
{
    ste_cat_cc_dialled_address_t   smsc;  /**< The type of CC */
    ste_cat_cc_dialled_address_t   dest;  /**< The SMS destination address */
} ste_cat_sms_control_t;


/**
 * @brief	Request CAT to perform SMS control.
 * @param cat   Object to operate on.
 * @param client_tag The client tag that will be returned in the callback function.
 * @param sc_p Pointer to the cat sms control object
 * @return      Status of the operation.  0 on success, -1 on failure.
 *
 */

int                     ste_cat_sms_control(ste_sim_t             * cat,
                                            uintptr_t               client_tag,
                                            ste_cat_sms_control_t * sc_p);


//The response for call control
/** Call Control result */
typedef enum
{
    STE_CAT_CC_ALLOWED_NO_MODIFICATION    = 0,
    STE_CAT_CC_NOT_ALLOWED                = 1,
    STE_CAT_CC_ALLOWED_WITH_MODIFICATIONS = 2,
    STE_CAT_CC_NOT_ALLOWED_TEMP_PROBLEM   = 3
} ste_cat_call_control_result_t;

/**
 * @brief Data structure holding EC call control response.
 *
 * This is used to pass status into the callback function when the cause is
 * STE_CAT_CAUSE_SIM_EC_CALL_CONTROL.
 */
typedef struct {
    ste_cat_call_control_result_t  cc_result;        /**< Result of the CALL CONTROL service */
    ste_cat_call_control_t         cc_info;          /**< The call control result related info */
    ste_sim_string_t               user_indication;  /**< User indication for this call control */
} ste_cat_call_control_response_t;

/**
 * @brief CAT timing advance response struct, paramters: Information validity and actual timing advance value.
 */

typedef struct {
    uint8_t status; /**< Timing advance information validity */
    uint8_t value;  /**< Timing advance value */
} ste_cat_timing_advance_response_t;

//The response for sms control
/** sms Control result */
typedef ste_cat_call_control_result_t ste_cat_sms_control_result_t;

/**
 * @brief Data structure holding EC sms control response.
 *
 * This is used to pass status into the callback function when the cause is
 * STE_CAT_CAUSE_SIM_EC_SMS_CONTROL.
 */
typedef struct {
    ste_cat_sms_control_result_t  sc_result;        /**< Result of the SMS CONTROL service */
    ste_cat_sms_control_t         sc_info;          /**< The sms control result related info */
    ste_sim_string_t              user_indication;  /**< User indication for this sms control */
} ste_cat_sms_control_response_t;


#define SIM_MAX_PATH_LENGTH                       (10)
#define SIM_FILE_MAX_FCP_LENGTH                   (256)
#define SIM_EF_IMSI_LEN                           (9)
#define SIM_EF_IMSI_STRING_LEN                    (16)

typedef enum
{
  UICC_REQUEST_STATUS_OK,
  UICC_REQUEST_STATUS_PENDING,
  UICC_REQUEST_STATUS_NOT_AVAILABLE,
  UICC_REQUEST_STATUS_FAILED,
  UICC_REQUEST_STATUS_FAILED_RESOURCE,
  UICC_REQUEST_STATUS_TIME_OUT,
  UICC_REQUEST_STATUS_FAILED_PARAMETER,
  UICC_REQUEST_STATUS_FAILED_APPLICATION,
  UICC_REQUEST_STATUS_FAILED_UNPACKING,
  UICC_REQUEST_STATUS_FAILED_STATE
} uicc_request_status_t;


/**
 * Reason codes for sim_status event.
 * Note the following: After appl PIN has been verified at startup,
 * the PIN_VERIFIED reason is NOT reported! Instead, the module reports
 * STARTUP_DONE reason.
 */

typedef enum {
  SIM_REASON_UNKNOWN = 0, // Msg for no particular reason
  SIM_REASON_STARTUP_DONE,
  SIM_REASON_PIN_NEEDED,
  SIM_REASON_PIN_VERIFIED,
  SIM_REASON_PUK_NEEDED,
  SIM_REASON_PIN2_NEEDED,
  SIM_REASON_PIN2_VERIFIED,
  SIM_REASON_PUK2_NEEDED,
  SIM_REASON_DISCONNECTED_CARD,
  // Error sort of reason codes
  SIM_REASON_PERMANENTLY_BLOCKED = 100,
  SIM_REASON_NO_CARD,
  SIM_REASON_REJECTED_CARD_INVALID,
  SIM_REASON_REJECTED_CARD_SIM_LOCK,
  SIM_REASON_REJECTED_CARD_CONSECUTIVE_6F00
} sim_reason_t;

/**
 * @brief Struct returned with STE_UICC_CAUSE_SIM_STATUS
 */

typedef struct {
  sim_reason_t reason; /**< Reason returned with STE_UICC_CAUSE_SIM_STATUS. */
} sim_status_t;

/**
 * @brief Structure used by the response functions for the SIM I/O and PIN requests
 */

typedef enum
{
  STE_UICC_STATUS_CODE_OK = 0,             /**< The UICC request was performed successfully */
  STE_UICC_STATUS_CODE_FAIL,               /**< The UICC request failed. See status code details parameter for further details */
  STE_UICC_STATUS_CODE_UNKNOWN,            /**< Status is unknown */
  STE_UICC_STATUS_CODE_NOT_READY,          /**< The UICC Server is not ready */
  STE_UICC_STATUS_CODE_SHUTTING_DOWN,      /**< The UICC server could not serve the request since it is shutting down */
  STE_UICC_STATUC_CODE_CARD_READY,         /**< The card is ready */
  STE_UICC_STATUS_CODE_CARD_NOT_READY,     /**< The UICC server could not serve the request since the card is not ready */
  STE_UICC_STATUS_CODE_CARD_DISCONNECTED,  /**< The UICC server could not serve the request since the card is disconnected */
  STE_UICC_STATUS_CODE_CARD_NOT_PRESENT,   /**< The UICC server could not serve the request since the card is not present */
  STE_UICC_STATUS_CODE_CARD_REJECTED,      /**< The UICC server could not serve the request since the card has been rejected */
  STE_UICC_STATUS_CODE_APPL_ACTIVE,        /**< The application is active */
  STE_UICC_STATUS_CODE_APPL_ACTIVE_PIN,    /**< The application is active and needs PIN*/
  STE_UICC_STATUS_CODE_APPL_ACTIVE_PUK,    /**< The application is active and needs PUK*/
  STE_UICC_STATUS_CODE_APPL_NOT_ACTIVE,    /**< The application is not active */
  STE_UICC_STATUS_CODE_PIN_ENABLED,        /**< PIN verification is enabled */
  STE_UICC_STATUS_CODE_PIN_ENABLED_NOT_VERIFIED,  /**< PIN verification is enabled but not verified */
  STE_UICC_STATUS_CODE_PIN_ENABLED_VERIFIED, /**< PIN verification is enabled and verified */
  STE_UICC_STATUS_CODE_PIN_ENABLED_BLOCKED,  /**< PIN verification is enabled but PUK needed */
  STE_UICC_STATUS_CODE_PIN_ENABLED_PERM_BLOCKED,  /**< PIN verification is enabled but perm blocked */
  STE_UICC_STATUS_CODE_PIN_DISABLED        /**< PIN verification is disabled */
} ste_uicc_status_code_t;

/** @brief Structure used by the response function for the SIM I/O and PIN requests
 *
 *  This structure is used together with the ste_uicc_status_code_t in response
 *  data types. The values in parameters of this type is only valid is the
 *  status code parameter is set to STE_UICC_STATUS_CODE_FAIL.
 */
typedef enum
{
  STE_UICC_STATUS_CODE_FAIL_DETAILS_NO_DETAILS = 0,                     /**< No details are available */
  STE_UICC_STATUS_CODE_FAIL_DETAILS_INVALID_PARAMETERS,                 /**< The parameters supplied in the request are invalid */
  STE_UICC_STATUS_CODE_FAIL_DETAILS_FILE_NOT_FOUND,                     /**< The requested file was not found */
  STE_UICC_STATUS_CODE_FAIL_DETAILS_SECURITY_CONDITIONS_NOT_SATISFIED,  /**< The caller has insufficient access rights for the request */
  STE_UICC_STATUS_CODE_FAIL_DETAILS_CARD_ERROR,                         /**< Could not communicate with the card */
  STE_UICC_STATUS_CODE_FAIL_DETAILS_SERVICE_NOT_SUPPORTED,              /**< The request is currently not supported */
  STE_UICC_STATUS_CODE_FAIL_DETAILS_INTERNAL_ERROR                      /**< An internal error has occured */
} ste_uicc_status_code_fail_details_t;

#define SIM_EF_IMSI (0x6F07)
#define SIM_EF_ADN (0x6F3A)

typedef enum
{
  SIM_EF_UNKNOWN,
  SIM_EF_DIR,
  SIM_EF_PL,
  SIM_EF_ICC_ID,
  SIM_EF_LP,
  //  SIM_EF_IMSI = 0x6f07,
  SIM_EF_KEYS,
  SIM_EF_KEYS_PS,
  SIM_EF_CPHS_MWF,
  SIM_EF_CPHS_CFF,
  SIM_EF_CPHS_OPERATOR_NAME,
  SIM_EF_CPHS_CSP,
  SIM_EF_CPHS_INFO,
  SIM_EF_CPHS_MAILBOX,
  SIM_EF_CPHS_OPERATOR_NAME_SHORTFORM,
  SIM_EF_CPHS_INFO_NUMBERS,
  SIM_EF_KC,
  SIM_EF_DCK,
  SIM_EF_PLMN_SEL,
  SIM_EF_HPPLMN,
  SIM_EF_CNL,
  SIM_EF_ACM_MAX,
  SIM_EF_SERVICE_TABLE,
  SIM_EF_ACM,
  SIM_EF_SMS,
  SIM_EF_SMSP,
  SIM_EF_SMSS,
  SIM_EF_SMS_STATUS_REPORT,
  SIM_EF_GID_1,
  SIM_EF_GID_2,
  // SIM_EF_ADN = 0x6F3A,
  SIM_EF_FDN,
  SIM_EF_MSISDN,
  SIM_EF_SDN,
  SIM_EF_BDN,
  SIM_EF_LND,
  SIM_EF_EXT_1,
  SIM_EF_EXT_2,
  SIM_EF_EXT_3,
  SIM_EF_EXT_4,
  SIM_EF_EXT_5,
  SIM_EF_CCP,
  SIM_EF_CCP2_ECCP,
  SIM_EF_PUCT,
  SIM_EF_CBMI,
  SIM_EF_SPN,
  SIM_EF_CBMID,
  SIM_EF_CBMIR,
  SIM_EF_KC_GPRS,
  SIM_EF_LOCI_GPRS,
  SIM_EF_EST,
  SIM_EF_ACL,
  SIM_EF_CMI,
  SIM_EF_START_HFN,
  SIM_EF_THRESHOLD,
  SIM_EF_PLMN_WACT,
  SIM_EF_OPLMN_WACT,
  SIM_EF_HPLMN_WACT,
  SIM_EF_CPBCCH,
  SIM_EF_INVSCAN,
  SIM_EF_PS_LOCI,
  SIM_EF_BCCH,
  SIM_EF_ACC,
  SIM_EF_FPLMN,
  SIM_EF_LOCI,
  SIM_EF_ICI,
  SIM_EF_OCI,
  SIM_EF_ICT,
  SIM_EF_OCT,
  SIM_EF_AD_USIM,
  SIM_EF_PHASE,
  SIM_EF_VGCS,
  SIM_EF_VGCSS,
  SIM_EF_VBS,
  SIM_EF_VBSS,
  SIM_EF_EMLPP,
  SIM_EF_AAEM,
  SIM_EF_ECC,
  SIM_EF_HIDDEN_KEY,
  SIM_EF_NETPAR,
  SIM_EF_PNN,
  SIM_EF_OPL,
  SIM_EF_MBDN,
  SIM_EF_EXT_6,
  SIM_EF_MBI,
  SIM_EF_MWIS,
  SIM_EF_CFIS,
  SIM_EF_EXT_7,
  SIM_EF_SPDI,
  SIM_EF_MMSN,
  SIM_EF_EXT_8,
  SIM_EF_MMS_ICP,
  SIM_EF_MMS_UP,
  SIM_EF_MMS_UCP,
  SIM_EF_NIA,
  SIM_EF_VGCSCA,
  SIM_EF_VBSCA,
  SIM_EF_GBA_BP_USIM,
  SIM_EF_MSK,
  SIM_EF_MUK,
  SIM_EF_EQUIVALENT_HPLMN,
  SIM_EF_GBA_NL,
  SIM_EF_ZONE_CELL_INFO,
  SIM_EF_LOCAL_CALLING_DETAILS,
  SIM_EF_LOCAL_PHONEBOOK_PSC,
  SIM_EF_LOCAL_PHONEBOOK_CC,
  SIM_EF_LOCAL_PHONEBOOK_PUID,
  SIM_EF_LOCAL_PHONEBOOK_PBR,
  SIM_EF_LOCAL_PHONEBOOK_FILE,
  SIM_EF_MEXE_ST,
  SIM_EF_MEXE_ORPK,
  SIM_EF_MEXE_ARPK,
  SIM_EF_MEXE_TPRPK,
  SIM_EF_MEXE_DATA_FILE,
  SIM_EF_WLAN_PSEUDONYM,
  SIM_EF_WLAN_USER_PLMN,
  SIM_EF_WLAN_OPERATOR_PLMN,
  SIM_EF_WLAN_USER_SPECIFIC_ID_LIST,
  SIM_EF_WLAN_OPERATOR_SPECIFIC_ID_LIST,
  SIM_EF_WLAN_REAUTHENTICATION_ID,
  SIM_EF_SOLSA_ACCESS_INDICATOR,
  SIM_EF_SOLSA_LSA_LIST,
  SIM_EF_SOLSA_LSA_DESCRIPTOR,
  SIM_EF_INFO_NR,
  SIM_EF_GLOBAL_PHONEBOOK_PSC,
  SIM_EF_GLOBAL_PHONEBOOK_CC,
  SIM_EF_GLOBAL_PHONEBOOK_PUID,
  SIM_EF_GLOBAL_PHONEBOOK_PBR,
  SIM_EF_GLOBAL_PHONEBOOK_FILE,
  SIM_EF_MULTIMEDIA_MSG_LIST,
  SIM_EF_MULTIMEDIA_MSG_DATA_FILE,
  SIM_EF_IMAGE_FILE,
  SIM_EF_IMAGE_INSTANCE_DATA_FILE,
  SIM_EF_ORANGE_CARD_TYPE,
  SIM_EF_DYNAMIC2_FLAGS,
  SIM_EF_ORANGE_SERVICE_CONTROL_TABLE,
  SIM_EF_ZONAL_INDICATOR_1,
  SIM_EF_ZONAL_INDICATOR_2,
  SIM_EF_CSP_2,
  SIM_EF_JACOB,
  SIM_EF_ORANGE_PARAMETERS,
  SIM_EF_ACM_LINE_2,
  SIM_EF_DYNAMIC_FLAGS,
  SIM_EF_HZ_PARAMETERS,
  SIM_EF_HZ_CELL_CACHE_1,
  SIM_EF_HZ_CELL_CACHE_2,
  SIM_EF_HZ_CELL_CACHE_3,
  SIM_EF_HZ_CELL_CACHE_4,
  SIM_EF_UHZ_TAGS,
  SIM_EF_UHZ_SUBSCRIBED_LAC_CI_1,
  SIM_EF_UHZ_SUBSCRIBED_LAC_CI_2,
  SIM_EF_UHZ_SUBSCRIBED_LAC_CI_3,
  SIM_EF_UHZ_SUBSCRIBED_LAC_CI_4,
  SIM_EF_UHZ_SETTINGS,
  SIM_EF_ACTING_HPLMN,
  SIM_EF_IMEI_UPLOAD,
  SIM_EF_OLD_IMEI,
  SIM_EF_RAT_MODE,
  SIM_EF_AMBIGUOUS,
  SIM_EF_NOT_SUPPORTED
} sim_ef_t;

typedef enum
{
  SIM_RECORD_ID_ABSOLUTE_FIRST = 0x0001,
  SIM_RECORD_ID_ABSOLUTE_LAST  = 0x00FE,
  SIM_RECORD_ID_PREVIOUS       = 0x0100,
  SIM_RECORD_ID_CURRENT        = 0x0200,
  SIM_RECORD_ID_NEXT           = 0x0400
} sim_record_id_t;

typedef enum
{
  SIM_FILE_STRUCTURE_UNKNOWN,
  SIM_FILE_STRUCTURE_NO_INFO_GIVEN,
  SIM_FILE_STRUCTURE_DF,
  SIM_FILE_STRUCTURE_MF,
  SIM_FILE_STRUCTURE_TRANSPARENT,
  SIM_FILE_STRUCTURE_LINEAR_FIXED,
  SIM_FILE_STRUCTURE_CYCLIC,
  SIM_FILE_STRUCTURE_BER_TLV
} sim_file_structure_t;

/**
 * @brief Sim File format struct with number of records and file structures
 */

typedef struct
{
  uint8_t               num_recs;   /**< Number of records. */
  uint16_t              rec_len;    /**< Length of record. */
  sim_file_structure_t  structure;  /**< File structure. */
} sim_fileformat_t;

/**
 * @brief Sim path struct, parameters: path length and actual path.
 */

typedef struct
{
  uint8_t pathlen;                      /**< Length of Path. */
  uint8_t path[SIM_MAX_PATH_LENGTH];    /**< Actual Path */
} sim_path_t;

/**
 * @brief SIM states that are sent to the clients
 */

typedef enum
{
  SIM_STATE_UNKNOWN = 0,                    /**< Unknown state of the SIM card. */
  SIM_STATE_SIM_ABSENT,                     /**< SIM card is absent due to failure or detachment. */
  SIM_STATE_NOT_READY,                      /**< SIM card is not ready to accept any requests from client. */
  SIM_STATE_PIN_NEEDED,                     /**< SIM card requests for PIN1 verification. */
  SIM_STATE_READY,                          /**< SIM card is ready to serve client requests. */
  SIM_STATE_PIN2_NEEDED,                    /**< SIM card requests for PIN2 verification. */
  SIM_STATE_PUK_NEEDED,                     /**< SIM card requests for PUK verification. */
  SIM_STATE_PUK2_NEEDED,                    /**< SIM card requests for PUK2 verification. */
  SIM_STATE_SAP,                            /**< SIM card is serving the Bluetooth SIM Access Profile. */
  SIM_STATE_PERMANENTLY_BLOCKED,            /**< SIM card is permanently blocked since maximum number of retries reached. */
  SIM_STATE_REJECTED_CARD_INVALID,          /**< SIM card has been rejected due to invalid card */
  SIM_STATE_REJECTED_CARD_SIM_LOCK,         /**< SIM card has been rejected due to SIM Lock validation failure */
  SIM_STATE_REJECTED_CARD_CONSECUTIVE_6F00, /**< SIM card has been rejected due to three consecutive 6F00 status words from the SIM. */
  SIM_STATE_DISCONNECTED_CARD               /**< SIM card has been powered off. */
} sim_state_t;


/**
 * @brief Data structure used for the data parameter in the ste_uicc_callback
 *              when the cause is STE_UICC_CAUSE_REQ_PIN_VERIFY;
 */
typedef struct {
  ste_uicc_status_code_t              uicc_status_code;              /**< uicc_status_code. Status */
  ste_uicc_status_code_fail_details_t uicc_status_code_fail_details; /**< uicc_status_code_fail_details. Status details */
  ste_uicc_status_word_t              status_word;                   /**< status_word. The status word retrieved. */
} ste_uicc_pin_verify_response_t;

/**
 * @brief Data structure used for the data parameter in the ste_uicc_callback
 *              when the cause is STE_UICC_CAUSE_REQ_PIN_CHANGE;
 */
typedef struct {
  ste_uicc_status_code_t              uicc_status_code;              /**< uicc_status_code. Status */
  ste_uicc_status_code_fail_details_t uicc_status_code_fail_details; /**< uicc_status_code_fail_details. Status details */
  ste_uicc_status_word_t              status_word;                   /**< status_word. The status word retrieved. */
} ste_uicc_pin_change_response_t;

/**
 * @brief Data structure used for the data parameter in the ste_uicc_callback
 *              when the cause is STE_UICC_CAUSE_REQ_PIN_DISABLE;
 */
typedef struct {
  ste_uicc_status_code_t              uicc_status_code;              /**< uicc_status_code. Status */
  ste_uicc_status_code_fail_details_t uicc_status_code_fail_details; /**< uicc_status_code_fail_details. Status details */
  ste_uicc_status_word_t              status_word;                   /**< status_word. The status word retrieved. */
} ste_uicc_pin_disable_response_t;


/**
 * @brief Data structure used for the data parameter in the ste_uicc_callback
 *              when the cause is STE_UICC_CAUSE_REQ_PIN_ENABLE;
 */
typedef struct {
  ste_uicc_status_code_t              uicc_status_code;              /**< uicc_status_code. Status */
  ste_uicc_status_code_fail_details_t uicc_status_code_fail_details; /**< uicc_status_code_fail_details. Status details */
  ste_uicc_status_word_t              status_word;                   /**< status_word. The status word retrieved. */
} ste_uicc_pin_enable_response_t;

/**
 * @brief Status code included in the response data for a given request.
 * @param STE_UICC_SIM_STATUS_OK                       Ok status for SIM request.
 * @param STE_UICC_SIM_STATUS_FAIL                     Failure status for SIM request.
 * @param STE_UICC_SIM_STATUS_UNKNOWN                  Unknown SIM status.
 *
 * This data type is sent in the response data structure for UICC request.
 * It is used for determining the status of the given UICC request.
 *
 */
typedef enum {
  STE_UICC_SIM_STATUS_OK,
  STE_UICC_SIM_STATUS_FAIL,
  STE_UICC_SIM_STATUS_UNKNOWN
} ste_uicc_sim_status_t;

/**
 * @brief PIN status code included in the response data for the ste_uicc_pin_info request.
 * @param STE_UICC_PIN_STATUS_UNKNOWN                       Pin status is unknown.
 * @param STE_UICC_PIN_STATUS_ENABLED                       Pin status is enabled.
 * @param STE_UICC_PIN_STATUS_DISABLED                      Pin status is disabled.
 *
 * PIN status code which described if the PIN was enabled or disabled or if the status was unknown.
 * Included in the ste_uicc_pin_info_response_t response data that is used when the cause
 * is STE_UICC_CAUSE_REQ_PIN_INFO.
 *
 */
typedef enum {
  STE_UICC_PIN_STATUS_UNKNOWN,
  STE_UICC_PIN_STATUS_ENABLED,
  STE_UICC_PIN_STATUS_DISABLED
} ste_uicc_pin_status_t;

/**
 * @brief Data structure used for the data parameters in the ste_uicc_callback
 *              when the cause is STE_UICC_CAUSE_REQ_PIN_INFO;
 */
typedef struct {
  ste_uicc_status_code_t              uicc_status_code;              /**< uicc_status_code. Status */
  ste_uicc_status_code_fail_details_t uicc_status_code_fail_details; /**< uicc_status_code_fail_details. Status details */
  ste_uicc_pin_status_t               pin_status;                    /**< Data. parameters retrieved. */
  int                                 attempts_remaining;            /**< Data. parameters retrieved. */
} ste_uicc_pin_info_response_t;


/**
 * @brief Data structure used for the data parameter in the ste_uicc_callback
 *              when the cause is STE_UICC_CAUSE_REQ_PIN_UNBLOCK;
 */
typedef struct {
  ste_uicc_status_code_t              uicc_status_code;              /**< uicc_status_code. Status */
  ste_uicc_status_code_fail_details_t uicc_status_code_fail_details; /**< uicc_status_code_fail_details. Status details */
  ste_uicc_status_word_t              status_word;                   /**< status_word. The status word retrieved. */
} ste_uicc_pin_unblock_response_t;

/**
 * @brief Data structure used for the data parameter in the ste_uicc_callback
 *              when the cause is STE_UICC_CAUSE_REQ_READ_SIM_FILE_RECORD;
 */
typedef struct {
    ste_uicc_status_code_t              uicc_status_code;              /**< uicc_status_code. Status */
    ste_uicc_status_code_fail_details_t uicc_status_code_fail_details; /**< uicc_status_code_fail_details. Status details */
    ste_uicc_status_word_t              status_word;                   /**< status_word. The status word retrieved. */
    uint8_t *                           data;                          /**< data. record retrieved. */
    uint8_t                             length;                        /**< length. The length of the record */
} ste_uicc_sim_file_read_record_response_t;


/**
 * @brief Data structure for reading the IMSI record
 */
typedef struct {
    ste_uicc_status_code_t              uicc_status_code;              /**< uicc_status_code. Status */
    ste_uicc_status_code_fail_details_t uicc_status_code_fail_details; /**< uicc_status_code_fail_details. Status details */
    ste_uicc_status_word_t              status_word;                   /**< status_word. The status word retrieved. */
    uint8_t                             raw_imsi[SIM_EF_IMSI_LEN];     /**< raw binary data read from the file. */
    char                                imsi[SIM_EF_IMSI_STRING_LEN];  /**< converted imsi string, that can be printed. */
} ste_uicc_sim_file_read_imsi_response_t;

/**
 * @brief Data structure for SMSC
 */
typedef struct {
    ste_sim_text_t               num_text;  /**< the text structure holds the number part. */
    ste_sim_type_of_number_t     ton;       /**< type of number. */
    ste_sim_numbering_plan_id_t  npi;       /**< numbering plan id. */
} ste_sim_call_number_t;

/**
 * @brief Data structure used for the data parameter in the ste_uicc_callback
 *              when the cause is STE_UICC_CAUSE_REQ_READ_SIM_FILE_BINARY;
 */
typedef struct {
    ste_uicc_status_code_t              uicc_status_code;              /**< uicc_status_code. Status */
    ste_uicc_status_code_fail_details_t uicc_status_code_fail_details; /**< uicc_status_code_fail_details. Status details */
    ste_uicc_status_word_t              status_word;                   /**< status_word. The status word retrieved. */
    uint8_t *                           data;                          /**< data. record retrieved. */
    uint16_t                            length;                        /**< length. The length of the file */
} ste_uicc_sim_file_read_binary_response_t;

/**
 * @brief Data structure used for the data parameter in the ste_uicc_callback
 *              when the cause is STE_UICC_CAUSE_REQ_SIM_FILE_READ_GENERIC;
 */
typedef struct {
    ste_uicc_status_code_t              uicc_status_code;              /**< uicc_status_code. Status */
    ste_uicc_status_code_fail_details_t uicc_status_code_fail_details; /**< uicc_status_code_fail_details. Status details */
    ste_uicc_status_word_t              status_word;                   /**< status_word. The status word retrieved. */
    int                                 file_id;                       /**< the requested file id. */
    uint8_t *                           data_p;                        /**< the data read from the file. */
    uint8_t                             data_len;                      /**< length of the data. */
} ste_uicc_sim_file_read_generic_response_t;

/**
 * @brief Data structure used for the data parameter in the ste_uicc_callback
 *              when the cause is STE_UICC_CAUSE_REQ_UPDATE_EF_RECORD;
 */
typedef struct {
  ste_uicc_status_code_t              uicc_status_code;              /**< uicc_status_code. Status */
  ste_uicc_status_code_fail_details_t uicc_status_code_fail_details; /**< uicc_status_code_fail_details. Status details */
  ste_uicc_status_word_t              status_word;                   /**< status_word. The status word retrieved. */
} ste_uicc_update_sim_file_record_response_t;

/**
 * @brief Data structure used for the data parameter in the ste_uicc_callback
 *              when the cause is STE_UICC_CAUSE_REQ_UPDATE_EF_BINARY;
 */
typedef struct {
  ste_uicc_status_code_t              uicc_status_code;              /**< uicc_status_code. Status */
  ste_uicc_status_code_fail_details_t uicc_status_code_fail_details; /**< uicc_status_code_fail_details. Status details */
  ste_uicc_status_word_t              status_word;                   /**< status_word. The status word retrieved. */
} ste_uicc_update_sim_file_binary_response_t;

/**
 * @brief Data structure used for the data parameter in the ste_uicc_callback
 *              when the cause is STE_UICC_CAUSE_REQ_GET_FILE_FORMAT;
 */
typedef struct {
  ste_uicc_status_code_t              uicc_status_code;              /**< uicc_status_code. Status */
  ste_uicc_status_code_fail_details_t uicc_status_code_fail_details; /**< uicc_status_code_fail_details. Status details */
  ste_uicc_status_word_t              status_word;                   /**< status_word. The status word retrieved. */
  sim_file_structure_t                file_type;                     /**< file_type. File type */
  int                                 file_size;                     /**< file_size. File size. */
  int                                 record_len;                    /**< record_len. Record Length */
  int                                 num_records;                   /**< num_records. Number of records */
} ste_uicc_sim_file_get_format_response_t;

/**
 * @brief Data structure used for the data parameter in the ste_uicc_callback
 *              when the cause is STE_UICC_CAUSE_REQ_GET_FILE_INFORMATION;
 */
typedef struct {
  ste_uicc_status_code_t              uicc_status_code;              /**< uicc_status_code. Status */
  ste_uicc_status_code_fail_details_t uicc_status_code_fail_details; /**< uicc_status_code_fail_details. Status details */
  ste_uicc_status_word_t              status_word;                   /**< status_word. The status word retrieved. */
  uint8_t                            *fcp;                           /**< File Control Parameters as defined by 3GPP 31.102. */
  int                                 length;                        /**< Length of fcp. */
} ste_uicc_get_file_information_response_t;


/**
 * @brief Data structure used for the data parameter in the ste_uicc_callback
 *              when the cause is STE_UICC_CAUSE_REQ_APPL_APDU_SEND;
 */
typedef struct {
    ste_uicc_status_code_t              uicc_status_code;              /**< uicc_status_code. Status */
    ste_uicc_status_code_fail_details_t uicc_status_code_fail_details; /**< uicc_status_code_fail_details. Status details */
    ste_uicc_status_word_t              status_word;                   /**< status_word. The status word retrieved. */
    uint8_t *                           data;                          /**< data. record retrieved. */
    uint16_t                            length;                        /**< length. The length of the file */
} ste_uicc_appl_apdu_send_response_t;


/**
 * @brief Data structure used for informing sim clients about a change of sim state.
 *             when the cause is STE_UICC_CAUSE_REQ_GET_FILE_INFORMATION;
 */
typedef struct {
    int             error_cause;    /**< the error info returned. */
    sim_state_t     state;          /**< The SIM State. */
} ste_uicc_get_sim_state_response_t;


/**
 * @brief Data structure used for the data parameter in the ste_uicc_callback
 *              when the cause is STE_UICC_CAUSE_REQ_UPDATE_SERVICE_TABLE;
 */
typedef struct {
  ste_uicc_status_code_t              uicc_status_code;              /**< uicc_status_code. Status */
  ste_uicc_status_code_fail_details_t uicc_status_code_fail_details; /**< uicc_status_code_fail_details. Status details */
  ste_uicc_status_word_t              status_word;                   /**< status_word. The status word retrieved. */
} ste_uicc_update_service_table_response_t;



/**
 * @brief Service status code included in the response data for the ste_uicc_get_service_table request.
 *
 * Service status code which tells if the service is enabled, disabled or if the status is unknown.
 * Included in the ste_uicc_get_service_table_response_t response data that is used when the cause
 * is STE_UICC_CAUSE_REQ_GET_SERVICE_TABLE.
 *
 */
typedef enum {
  STE_UICC_SERVICE_STATUS_UNKNOWN,
  STE_UICC_SERVICE_STATUS_ENABLED,
  STE_UICC_SERVICE_STATUS_DISABLED
} ste_uicc_service_status_t;


/**
 * @brief Data structure used for the data parameters in the ste_uicc_callback
 *              when the cause is STE_UICC_CAUSE_REQ_GET_SERVICE_TABLE;
 */
typedef struct {
  ste_uicc_status_code_t              uicc_status_code;              /**< uicc_status_code. Status */
  ste_uicc_status_code_fail_details_t uicc_status_code_fail_details; /**< uicc_status_code_fail_details. Status details */
  ste_uicc_status_word_t              status_word;                   /**< status_word. The status word retrieved. */
  ste_uicc_service_status_t           service_status;                /**< Data. parameters retrieved. */
} ste_uicc_get_service_table_response_t;


/**
 * @brief Service status code included in the response data for the ste_uicc_get_service_availability request.
 *
 * Service status code which tells if the service is available, not available or if the availability is unknown.
 * Included in the ste_uicc_get_service_availability_response_t response data that is used when the cause
 * is STE_UICC_CAUSE_REQ_GET_SERVICE_AVAILABILITY.
 *
 */
typedef enum {
  STE_UICC_SERVICE_AVAILABILITY_UNKNOWN,
  STE_UICC_SERVICE_AVAILABLE,
  STE_UICC_SERVICE_NOT_AVAILABLE
} ste_uicc_service_availability_t;


/**
 * @brief Data structure used for the data parameters in the ste_uicc_callback
 *              when the cause is STE_UICC_CAUSE_REQ_GET_SERVICE_AVAILABILITY;
 */
typedef struct {
  ste_uicc_status_code_t              uicc_status_code;              /**< uicc_status_code. Status */
  ste_uicc_status_code_fail_details_t uicc_status_code_fail_details; /**< uicc_status_code_fail_details. Status details */
  ste_uicc_service_availability_t     service_availability;          /**< Data. parameters retrieved. */
} ste_uicc_get_service_availability_response_t;


/**
 * @brief Data structure used for informing sim clients about a change of sim state.
 *        This is the data structure which contains information about the unsolicited
 *        event related to STE_UICC_CAUSE_SIM_STATE_CHANGED.
 */
typedef struct {
    sim_state_t             state;              /**< The SIM State returned by the card. */
} ste_uicc_sim_state_changed_t;

/**
 * @brief Data structure for the not ready response
 */
typedef struct {
    uint32_t type;                              /**< The type of not ready response. */
} ste_uicc_not_ready_t;

/**
 * @brief Various application types returned by the SIM card
 * @param SIM_APP_UNKNOWN                       SIM application type unknown.
 * @param SIM_APP_GSM                           SIM application type GSM.
 * @param SIM_APP_USIM                          SIM application type USIM.
 * @param SIM_APP_ISIM                          SIM application type ISIM.
 */
typedef enum {
  SIM_APP_UNKNOWN,
  SIM_APP_GSM,
  SIM_APP_USIM,
  SIM_APP_ISIM
} sim_app_type_t;

/**
 * @brief Data structure used for describing which type of PIN code that is used
 * @param SIM_PIN_ID_PIN1 Identifier for PIN1
 * @param SIM_PIN_ID_PIN2 Identifier for PIN2
 */
typedef enum {
  SIM_PIN_ID_PIN1 = 0,
  SIM_PIN_ID_PIN2,
  SIM_PIN_ID_UNKNOWN
} sim_pin_id_t;

/**
 * @brief Data structure used for describing which type of PIN code that is used
 * @param SIM_PIN_PIN1 Identifier for PIN1
 * @param SIM_PIN_PUK1 Identifier for PUK1
 * @param SIM_PIN_PIN2 Identifier for PIN2
 * @param SIM_PIN_PUK2 Identifier for PUK2
 */
typedef enum {
  SIM_PIN_PIN1 = 0,
  SIM_PIN_PUK1,
  SIM_PIN_PIN2,
  SIM_PIN_PUK2
} sim_pin_puk_id_t;

/**
 * @brief Structure used to return information about current sim application
 */
typedef struct {
  int                  status;                /**< the error info returned. */
  sim_app_type_t       app_type;              /**< The type of current sim application. */
} ste_uicc_sim_app_info_response_t;

/**
 * @brief A type to indicate to \ref ste_uicc_sim_get_file_information
 *        the kind of information to be read.
 */
typedef enum {
  STE_UICC_SIM_GET_FILE_INFO_TYPE_EF,      /**< flag to indicate reading EF */
  STE_UICC_SIM_GET_FILE_INFO_TYPE_DF,      /**< flag to indicate reading DF */
} ste_uicc_sim_get_file_info_type_t;

// enum of SAT refresh types
typedef enum
{
    STE_SIM_PC_REFRESH_INIT_AND_FULL_FILE_CHANGE = 0x00,
    STE_SIM_PC_REFRESH_FILE_CHANGE,
    STE_SIM_PC_REFRESH_INIT_AND_FILE_CHANGE,
    STE_SIM_PC_REFRESH_INIT,
    STE_SIM_PC_REFRESH_UICC_RESET,
    STE_SIM_PC_REFRESH_3G_APP_RESET,
    STE_SIM_PC_REFRESH_3G_SESSION_RESET,
    STE_SIM_PC_REFRESH_RESERVED
} ste_sim_pc_refresh_type_t;

typedef enum
{
     STE_CAT_REFRESH_RESULT_OK,
     STE_CAT_REFRESH_RESULT_FAIL
} ste_cat_refresh_result_t;

/**
 * @brief This message contains the refresh type
 */
typedef struct {
    uintptr_t                   simd_tag;   /**< The client tag generated by SIMD, can be NULL */
    ste_sim_pc_refresh_type_t   type;       /**< The kind of PC refresh being performed */
} ste_cat_pc_refresh_ind_t;

/**
 * @brief This message contains a file path of a file to be refreshed.
 */
typedef struct {
    uintptr_t                   simd_tag;   /**< The client tag generated by SIMD, can be NULL */
    sim_path_t                  path;       /**< The path of a file that has changed */
} ste_cat_pc_refresh_file_ind_t;

/**
 * @brief File identifier to use when updating PLMN
 */
typedef enum {
    SIM_FILE_ID_PLMN_WACT, /* PLMNwact 6F60*/
    SIM_FILE_ID_PLMN_SEL,  /* PLMNsel 6F30*/
}ste_uicc_sim_plmn_file_id_t;

/**
 * @brief PLMN (MCC + MNC) representation according to 3GPP 24.008
 */
typedef struct
{
unsigned int MCC1:4; /**< MCC1 Mobile Country Code - Nibble 1 */
unsigned int MCC2:4; /**< MCC2 Mobile Country Code - Nibble 2 */
unsigned int MCC3:4; /**< MCC3 Mobile Country Code - Nibble 3 */
unsigned int MNC3:4; /**< MNC1 Mobile Network Code - Nibble 3 */
unsigned int MNC1:4; /**< MNC2 Mobile Network Code - Nibble 1 */
unsigned int MNC2:4; /**< MNC3 Mobile Network Code - Nibble 2 */
} ste_uicc_sim_plmn_t;

/**
 * Access Technology Identifier bitmask. Coding according to 3GPP TS 31.102.
 * Bitmask that can be used to unmask specific access technology from ste_uicc_sim_plmn_AccessTechnology_t.
 */
typedef enum
{
SIM_ACT_UTRAN_BITMASK       = 0x0080,
SIM_ACT_GSM_COMPACT_BITMASK = 0x4000,
SIM_ACT_GSM_BITMASK         = 0x8000,
SIM_ACT_UNSPECIFIED_BITMASK = 0x0000,
} ste_uicc_sim_AcT_bitmask_t;

/**
 * Access Technology Identifier. Coding according to 3GPP TS 31.102.
 * bit 16 (0x8000) indicates GSM
 * bit 15 (0x4000) indicates GSM compact
 * bit 8  (0x80)   indicates UTRAN
 */
typedef uint16_t ste_uicc_sim_plmn_AccessTechnology_t;

/**
 * @brief Structure contain PLMN value and corresponding Access Technology
 */
typedef struct {
  ste_uicc_sim_plmn_t                  PLMN; /**< PLMN */
  ste_uicc_sim_plmn_AccessTechnology_t AcT;  /**< Access Technology. Default value,
                                              * (GSM and UTRAN enabled, GSM compact disabled (0x8080)), will be
                                              * returned when reading PLMNsel (6F30).*/
} ste_sim_plmn_with_AcT_t;

/**
 * @brief Data structure for reading the transparent PLMN files (6F60 & 6F30)
 */
typedef struct {
    ste_uicc_status_code_t              uicc_status_code;               /**< uicc_status_code. Status */
    ste_uicc_status_code_fail_details_t uicc_status_code_fail_details;  /**< uicc_status_code_fail_details. Status details */
    ste_uicc_status_word_t              status_word;                    /**< status_word. The status word retrieved. */
    ste_sim_plmn_with_AcT_t            *ste_sim_plmn_with_AcT_p;        /**< the structure to hold the result for reading plmn. */
    int                                 nr_of_plmn;                     /**< Number of PLMNs returned */
} ste_uicc_sim_file_read_plmn_response_t;

/**
 * @brief Data structure used for the data parameter in the ste_uicc_callback when updating the PLMN file.
 * Used when the cause is STE_UICC_CAUSE_REQ_SIM_FILE_UPDATE_PLMN
 */
typedef struct {
  ste_uicc_status_code_t              uicc_status_code;              /**< uicc_status_code. Status */
  ste_uicc_status_code_fail_details_t uicc_status_code_fail_details; /**< uicc_status_code_fail_details. Status details */
  ste_uicc_status_word_t              status_word;                   /**< status_word. The status word retrieved. */
} ste_uicc_update_sim_file_plmn_response_t;

// The application shall call this function when refresh is complete
int ste_cat_refresh_result(ste_sim_t                  * cat,
                           uintptr_t                    client_tag,
                           ste_sim_pc_refresh_type_t    type,
                           ste_cat_refresh_result_t     result);

/**
 * @brief	     Register with the uicc server as a uicc client.
 * @param uicc       Object to operate on.
 * @param client_tag The client tag that will be returned in the callback function.
 * @return           Status of the operation.  0 on success, -1 on failure.
 *
 * The client registers with the uicc server that it will accept receiving
 * requests from it.
 * The closure callback function us called with data set to 0 and the cause to
 * STE_UICC_CAUSE_REGISTER when the server acknowledges registration.
 * N.B. The return status of this function reflects the status of sending the
 * underlying message to the server only.
 */
int                     ste_uicc_register(ste_sim_t * uicc,
                                          uintptr_t client_tag);

/**
 * @brief	     Send the pin code to the uicc server in response to the
 *                   pin request.
 * @param uicc       Object to operate on.
 * @param client_tag The client tag that will be returned in the callback function.
 * @param pin_id     The pin_id of the pin being verified.
 * @param pin        Pointer to buffer where the PIN is stored.
 * @return           Status of the operation.  0 on success, -1 on failure.
 *
 * A response message is sent from the server.
 * N.B. The return status of this function reflects the status of sending the
 * underlying message to the server only.
 *
 * This should be used after a modem specific pin code request has been
 * received from the server.  The closure callback function is called with
 * cause STE_UICC_IND_REQDATA and data points to a structure containing a
 * buffer holding the request.
 */
int                     ste_uicc_pin_verify(ste_sim_t * uicc,
                                            uintptr_t client_tag,
                                            const sim_pin_id_t pin_id,
                                            const char *pin);

/**
 * @brief	     Send the current and the new pin code to the uicc server to request a pin change.
 * @param uicc       Object to operate on.
 * @param client_tag The client tag that will be returned in the callback function.
 * @param old_pin    Pointer to buffer where the current PIN is stored.
 * @param new_pin    Pointer to buffer where the new PIN is stored.
 * @param pin_id     The pin_id of the pin being changed.
 * @return           Status of the operation.  0 on success, -1 on failure.
 *
 * A response message is sent from the server.
 * N.B. The return status of this function reflects the status of sending the
 * underlying message to the server only.
 *
 * This should be used after a modem specific pin code has been verified by
 * the server.  The closure callback function is called with
 * cause STE_UICC_IND_REQDATA and data points to a structure containing a
 * buffer holding the request.
 */
int ste_uicc_pin_change(ste_sim_t * uicc,
                        uintptr_t client_tag,
                        const char *old_pin,
                        const char *new_pin,
                        const sim_pin_id_t pin_id);


/**
 * @brief	     Send a request, to disable the pin code, to the uicc server.
 * @param uicc       Object to operate on.
 * @param client_tag The client tag that will be returned in the callback function.
 * @param buf        Pointer to buffer where the PIN is stored.
 * @return           Status of the operation.  0 on success, -1 on failure.
 *
 * A response message is sent from the server.
 * N.B. The return status of this function reflects the status of sending the
 * underlying message to the server only.
 *
 * This should be used after a modem specific pin code has been verified by
 * the server.  The closure callback function is called with
 * cause STE_UICC_IND_REQDATA and data points to a structure containing a
 * buffer holding the request.
 */
int                     ste_uicc_pin_disable(ste_sim_t * uicc,
                                            uintptr_t client_tag,
                                            const char *buf);


/**
 * @brief	     Send a request, to enable the pin code, to the uicc server.
 * @param uicc       Object to operate on.
 * @param client_tag The client tag that will be returned in the callback function.
 * @param buf        Pointer to buffer where the PIN is stored.
 * @return           Status of the operation.  0 on success, -1 on failure.
 *
 * A response message is sent from the server.
 * N.B. The return status of this function reflects the status of sending the
 * underlying message to the server only.
 *
 * This should be used after a modem specific pin code has been verified by
 * the server.  The closure callback function is called with
 * cause STE_UICC_IND_REQDATA and data points to a structure containing a
 * buffer holding the request.
 */
int                     ste_uicc_pin_enable(ste_sim_t * uicc,
                                            uintptr_t client_tag,
                                            const char *buf);



/**
 * @brief            Send a request for pin info to the uicc server.
 * @param uicc       Object to operate on.
 * @param client_tag The client tag that will be returned in the callback function.
 * @param pin        The ID of the PIN/PUK to operate on.
 * @return           Status of the operation.  0 on success, -1 on failure.
 *
 * A response message is sent from the server.
 * N.B. The return status of this function reflects the status of sending the
 * underlying message to the server only.
 *
 * This should be used after a modem specific pin code has been verified by
 * the server.  The closure callback function is called with
 * cause STE_UICC_IND_REQDATA and data points to a structure containing a
 * buffer holding the request.
 *
 * Info : PIN/PUK status, PIN/PUK attempts left
 */
int                     ste_uicc_pin_info(ste_sim_t * uicc,
                                          uintptr_t client_tag,
                                          sim_pin_puk_id_t pin);


/**
 * @brief	     Send the pin unblocking code to the uicc server in response to the
 *                   pin unblock needed request.
 * @param uicc       Object to operate on.
 * @param client_tag The client tag that will be returned in the callback function.
 * @param pin_id     The pin_id of the pin being changed.
 * @param pin        Pointer to buffer where the PIN code is stored.
 * @param puk        Pointer to buffer where the PUK code is stored.
 * @return           Status of the operation.  0 on success, -1 on failure.
 *
 * A response message is sent from the server.
 * N.B. The return status of this function reflects the status of sending the
 * underlying message to the server only.
 *
 * This should be used after a modem specific pin code request has been
 * received from the server.  The closure callback function is called with
 * cause STE_UICC_IND_REQDATA and data points to a structure containing a
 * buffer holding the request.
 */
int                     ste_uicc_pin_unblock(ste_sim_t * uicc,
                                             uintptr_t client_tag,
                                             const sim_pin_id_t pin_id,
                                             const char *pin,
                                             const char *puk);

/**
 * @brief	     Used to obain the format of a specified SIM elementary file.
 * @param uicc       Object to operate on.
 * @param client_tag The client tag that will be returned in the callback function.
 * @param file_id    Record id of the record inside the file
 * @param file_path  Pointer to the path of the file.
 * @return       Status of the operation.
 *
 * A response message is sent from the server.
 * N.B. The return status of this function reflects the status of sending the
 * underlying message to the server only.
 *
 * This should be used before doing a file read operation.
 */

uicc_request_status_t         ste_uicc_sim_file_get_format(ste_sim_t *uicc,
                                                           uintptr_t client_tag,
                                                           int file_id,
                                                           const char *file_path);

/**
 * @brief                   Reads the specific SIM elementary file.
 * @param uicc              Object to operate on.
 * @param client_tag        The client tag that will be returned in the callback function.
 * @param file_id           File id of the file to read.
 * @param record_id         Record id of the record inside the file.
 * @param length            Length in bytes of data to read which is supplied by the client.
 *                          If length is set to 0 (zero) the record is read until end of record.
 * @param file_path         Pointer to the path of the file to be read.
 * @return                  Status of the operation.
 *
 * A response message is sent from the server.
 * N.B. The return status of this function reflects the status of sending the
 * underlying message to the server only.
 *
 * This read mechanism is record-based. There may be request to read one or more records.
 * In case of multiple records, the first record is returned in the response callback.
 * All other records are returned through successive callbacks. The data returned is encoded
 * according to 3GPP TS 51.011 and 31.102 as appropriate.
 */

uicc_request_status_t ste_uicc_sim_file_read_record(ste_sim_t * uicc,
                                                    uintptr_t client_tag,
                                                    int file_id,
                                                    int record_id,
                                                    int length,
                                                    const char *file_path);
/**
 * @brief	           Reads the specific transparent SIM elementary file.
 * @param uicc             Object to operate on.
 * @param client_tag       The client tag that will be returned in the callback function.
 * @param file_id          File id of the file to read.
 * @param offset           Offset in bytes into the file to be read.
 * @param length           Length in bytes of data to read which is
 *                         supplied by the client.
 * @param file_path        Pointer to the path of the file to be read.
 * @return                 Status of the operation.
 *
 * A response message is sent from the server.
 * N.B. The return status of this function reflects the status of sending the
 * underlying message to the server only.
 *
 * The client is responsible for setting the correct values of offset and
 * length. By setting an offset of zero and length equal to the length of
 * the file, as obtained by ste_uicc_sim_file_get_format, the whole file will
 * be read. The maximum length of data returned by the response
 * function is SIM_FILE_MAX_DATA_LENGTH. If the client sets a non-zero offset, data will
 * be read from that offset in the file. If the client sets a Length less than
 * that of the file, only that length of data shall be returned. The data
 * is encoded according to 3GPP TS 51.011 and 31.102 as appropriate.
 */

uicc_request_status_t ste_uicc_sim_file_read_binary(ste_sim_t *uicc,
						    uintptr_t client_tag,
						    int file_id,
						    int offset,
						    int length,
						    const char *file_path);

/**
 * @brief	           Writes to the specified transparent SIM elementary file.
 * @param uicc             Object to operate on.
 * @param client_tag       The client tag that will be returned in the callback function.
 * @param file_id          File id of the file to update.
 * @param offset           Offset in bytes into the file to be written.
 * @param length           Length in bytes of data to be written
 *                         to the file. This value is supplied by the client.
 * @param file_path        Pointer to the path of the file.
 * @param data             Pointer to the data that is to be written to the file.
 * @return                 Status of the operation.
 *
 * A response message is sent from the server.
 * N.B. The return status of this function reflects the status of sending the
 * underlying message to the server only.
 *
 * The data must be encoded by the client according to 3GPP TS 51.011 and
 * 31.102 as appropriate The client must set the correct values of offset and
 * length. The length of the file is obtained using ste_uicc_sim_file_get_format.
 * If the client wants to write the whole of a file in a single operation,
 * then the offset must be zero and the length must the length of the file.
 * The client may request a partial file update by setting other values for
 * offset and length. If the amount of data to be written exceeds SIM_FILE_MAX_DATA_LENGTH
 * then more than one signal will be required to transfer the data, however
 * this will be managed by the internal implementation.
 */

uicc_request_status_t ste_uicc_sim_file_update_binary(ste_sim_t *uicc,
						      uintptr_t client_tag,
						      int file_id,
						      int offset,
						      int length,
						      const char *file_path,
						      const uint8_t *data);

/**
 * @brief	           Writes to the specified SIM elementary file.
 * @param uicc             Object to operate on.
 * @param client_tag       The client tag that will be returned in the callback function.
 * @param file_id          File id of the file to update.
 * @param record_id        Record id of the record inside the file to update.
 * @param length           Length in bytes of data to be written
 *                         to the file. This value is supplied by the client.
 * @param file_path        Pointer to the path of the file.
 * @param data             Pointer to the data that is to be written to the file.
 * @return                 Status of the operation.
 *
 * A response message is sent from the server.
 * N.B. The return status of this function reflects the status of sending the
 * underlying message to the server only.
 *
 * This method allows updating only one record per request. The data must
 * be encoded by the client according to 3GPP TS 51.011 and 31.102 as appropriate.
 */


uicc_request_status_t ste_uicc_sim_file_update_record(ste_sim_t * uicc,
						      uintptr_t client_tag,
						      int file_id,
						      int record_id,
						      int length,
						      const char *file_path,
						      const uint8_t *data);


/**
 * @brief                  Returns the File Control Parameters (FCP).
 * @param uicc             Object to operate on.
 * @param client_tag       The client tag that will be returned in the callback function.
 * @param file_id          File id to use.
 * @param file_path        Optional pointer to the file path. It should be
 *                         supplied by the client to resolve an ambiguous EF.
 *                         It is ignored for a DF.
 * @param type             the type of information to return (EF or DF)
 * @return                 Status of the operation.
 *
 * A response message is sent from the server.
 * N.B. The return status of this function reflects the status of sending the
 * underlying message to the server only.
 *
 * The FCP is returned either for a specified Elementary File (EF) or for the
 * currently selected Dedicated File (DF) or Application DF (ADF); the MF is
 * regarded as a special case DF. The FCP data has a maximum length of
 * SIM_FILE_MAX_FCP_LENGTH and is encoded according to 3GPP TS 51.011 and
 * ETSI TS 102.221 as appropriate.
 *
 * The file_path and file_id cannot both be NULL, 0.
 */

uicc_request_status_t ste_uicc_sim_get_file_information(ste_sim_t * uicc,
                                                        uintptr_t client_tag,
                                                        int file_id,
                                                        const char *file_path,
                                                        ste_uicc_sim_get_file_info_type_t type);

/**
 * @brief	           Request for sim state.
 * @param uicc             Object to operate on.
 * @param client_tag       The client tag that will be returned in the callback function.
 * @return                 Status of the operation.
 *
 * A response message is sent from the server.
 * N.B. The return status of this function reflects the status of sending the
 * underlying message to the server only.
 *
 * Requests the state of the SIM.
 * Only one SIM State will be returned in the response signal, others (if they exist)
 * will be informed via the related callback. The client will need to make another call
 * to this method to get the correct state information.
 */


uicc_request_status_t         ste_uicc_sim_get_state(ste_sim_t * uicc,
                                                     uintptr_t client_tag);


/**
 * @brief                  Get information about the currently active uicc application
 * @param uicc             Object to operate on.
 * @param client_tag       The client tag passed from client, client tag that will be returned in the callback function.
 */

uicc_request_status_t ste_uicc_get_app_info(ste_sim_t * uicc,
					    uintptr_t client_tag);

/**
 * @brief	           Sync call for read binary file.
 * @param sim              Object to operate on.
 * @param client_tag       The client tag passed from client, client tag that will be returned in the callback function.
 * @param file_id          file id to be read.
 * @param offset           offset to read from.
 * @param length           length of the file to be read.
 * @param file_path        path for the file to be read.
 * @param read_result      structure to hold the read result.
 * @return                 Status of the operation.
 */
int ste_uicc_sim_file_read_binary_sync( ste_sim_t * sim,
                                        uintptr_t client_tag,
                                        int file_id,
                                        int offset,
                                        int length,
                                        const char * file_path,
                                        ste_uicc_sim_file_read_binary_response_t * read_result );

/**
 * @brief	           Sync call for read record file.
 * @param sim              Object to operate on.
 * @param client_tag       The client tag passed from client, client tag that will be returned in the callback function.
 * @param file_id          file id to be read.
 * @param record_id        the record id to be read on.
 * @param length           length of the file to be read.
 * @param file_path        path for the file to be read.
 * @param read_result      structure to hold the read result.
 * @return                 Status of the operation.
 */
int ste_uicc_sim_file_read_record_sync( ste_sim_t * sim,
                                        uintptr_t client_tag,
                                        int file_id,
                                        int record_id,
                                        int length,
                                        const char * file_path,
                                        ste_uicc_sim_file_read_record_response_t * read_result );

/**
 * @brief	           Sync call for update binary file.
 * @param sim              Object to operate on.
 * @param client_tag       The client tag passed from client, client tag that will be returned in the callback function.
 * @param file_id          file id to be updated.
 * @param offset           the offset of the file to be updated.
 * @param length           length of the file to be updated.
 * @param file_path        path for the file to be updated.
 * @param data             data to be put into the file.
 * @param update_result    structure to hold the update result.
 * @return                 Status of the operation.
 */
int ste_uicc_sim_file_update_binary_sync(ste_sim_t * sim,
                                         uintptr_t client_tag,
                                         int file_id,
                                         int offset,
                                         int length,
                                         const char * file_path,
                                         const uint8_t * data,
                                         ste_uicc_update_sim_file_binary_response_t * update_result );

/**
 * @brief	           Sync call for update record file.
 * @param sim              Object to operate on.
 * @param client_tag       The client tag passed from client, client tag that will be returned in the callback function.
 * @param file_id          file id to be updated.
 * @param record_id        the record id of the file to be updated.
 * @param length           length of the file to be updated.
 * @param file_path        path for the file to be updated.
 * @param data             data to be put into the file.
 * @param update_result    structure to hold the update result.
 * @return                 Status of the operation.
 */
int ste_uicc_sim_file_update_record_sync(ste_sim_t * sim,
                                         uintptr_t client_tag,
                                         int file_id,
                                         int record_id,
                                         int length,
                                         const char * file_path,
                                         const uint8_t * data,
                                         ste_uicc_update_sim_file_record_response_t * update_result );

/**
 * @brief	           Sync call for getting the format of a file.
 * @param sim              Object to operate on.
 * @param client_tag       The client tag passed from client, client tag that will be returned in the callback function.
 * @param file_id          file id to be operated on.
 * @param file_path        path for the file to be operated on.
 * @param file_format      structure to hold the format info of the file.
 * @return                 Status of the operation.
 */
int ste_uicc_sim_file_get_format_sync(ste_sim_t * sim,
                                      uintptr_t client_tag,
                                      int file_id,
                                      const char *file_path,
                                      ste_uicc_sim_file_get_format_response_t * file_format );

/**
 * @brief	           Sync call for getting the information of a file.
 * @param sim              Object to operate on.
 * @param client_tag       The client tag passed from client, client tag that will be returned in the callback function.
 * @param file_id          file id to be operated on.
 * @param path             path for the file to be operated on.
 * @param type             the type of information to return (EF or DF)
 * @param file_info        structure to hold the info of the file.
 * @return                 Status of the operation.
 */
int ste_uicc_sim_get_file_information_sync(ste_sim_t * sim,
                                           uintptr_t client_tag,
                                           int file_id,
                                           const char *path,
                                           ste_uicc_sim_get_file_info_type_t type,
                                           ste_uicc_get_file_information_response_t * file_info );

/**
 * @brief	           Sync call for reading the imsi file.
 * @param sim              Object to operate on.
 * @param client_tag       The client tag passed from client, client tag that will be returned in the callback function.
 * @param read_result      structure to hold the result of the reading.
 * @return                 Status of the operation.
 */
int ste_uicc_sim_file_read_specific_imsi_sync ( ste_sim_t * sim,
                                                uintptr_t client_tag,
                                                ste_uicc_sim_file_read_imsi_response_t * read_result );

/**
 * @brief Data structure for reading the active SMSC record
 */
typedef struct {
  ste_uicc_status_code_t              uicc_status_code;              /**< uicc_status_code. Status */
  ste_uicc_status_code_fail_details_t uicc_status_code_fail_details; /**< uicc_status_code_fail_details. Status details */
  ste_uicc_status_word_t              status_word;                   /**< status_word. The status word retrieved. */
  ste_sim_call_number_t               smsc;                          /**< the structure to hold the result for reading smsc. */
} ste_uicc_sim_smsc_get_active_response_t;

/**
 * @brief                  Async call for reading the active smsc parameters from the EFsmsp file.
 * @param uicc             Object to operate on.
 * @param client_tag       The client tag passed from client, client tag that will be returned in the callback function.
 * @return                 Status of the operation.
 */
uicc_request_status_t ste_uicc_sim_smsc_get_active ( ste_sim_t * uicc,
                                                     uintptr_t client_tag );

/**
 * @brief	           Sync call for reading the active smsc parameters from the EFsmsp file.
 * @param sim              Object to operate on.
 * @param client_tag       The client tag passed from client, client tag that will be returned in the callback function.
 * @param get_result       structure to hold the result of the reading.
 * @return                 Status of the operation.
 */
int ste_uicc_sim_smsc_get_active_sync ( ste_sim_t * sim,
                                        uintptr_t client_tag,
                                        ste_uicc_sim_smsc_get_active_response_t * get_result );

/**
 * @brief Data structure for setting the active SMSC record
 */
typedef struct {
  ste_uicc_status_code_t              uicc_status_code;              /**< uicc_status_code. Status */
  ste_uicc_status_code_fail_details_t uicc_status_code_fail_details; /**< uicc_status_code_fail_details. Status details */
  ste_uicc_status_word_t              status_word;                   /**< status_word. The status word retrieved. */
} ste_uicc_sim_smsc_set_active_response_t;

/**
 * @brief                  Async call for setting the active smsc parameters in the EFsmsp file.
 * @param uicc             Object to operate on.
 * @param client_tag       The client tag passed from client, client tag that will be returned in the callback function.
 * @param smsc_p           Pointer to the structure of the smsc number to be put into the file.
 * @return                 Status of the operation.
 */
uicc_request_status_t ste_uicc_sim_smsc_set_active ( ste_sim_t * uicc,
                                                     uintptr_t client_tag,
                                                     ste_sim_call_number_t * smsc_p );

/**
 * @brief                  Sync call for setting the active smsc parameters in the EFsmsp file.
 * @param sim              Object to operate on.
 * @param client_tag       The client tag passed from client, client tag that will be returned in the callback function.
 * @param smsc             structure of the smsc number to be put into the file.
 * @param set_result       structure to hold the result of the updating.
 * @return                 Status of the operation.
 */
int ste_uicc_sim_smsc_set_active_sync( ste_sim_t * sim,
                                       uintptr_t client_tag,
                                       ste_sim_call_number_t * smsc,
                                       ste_uicc_sim_smsc_set_active_response_t * set_result );

/** Response type used when the cause is the STE_UICC_CAUSE_REQ_SMSC_SAVE_TO_RECORD
 *
 *  @brief Data structure used for the data parameter in the ste_uicc_callback for ste_uicc_sim_smsc_save_to_record.
 *
 */
typedef struct {
  ste_uicc_status_code_t              uicc_status_code;              /**< uicc_status_code. Status */
  ste_uicc_status_code_fail_details_t uicc_status_code_fail_details; /**< uicc_status_code_fail_details. Status details */
  ste_uicc_status_word_t              status_word;                   /**< status_word. The status word retrieved. */
} ste_uicc_sim_smsc_save_to_record_response_t;

/** Saves the smsc parameters in the active record to the record identified by the record_id. The range of record id's
 *  that are valid are retrieved by calling ste_uicc_sim_smsc_get_record_max function. Record 1 and 2
 *  are reserved. Record 1 is used for storing the active SMSC parameters and record 2 is used for backing
 *  up the SMSC default parameters set by the manufacturer.
 *
 * @brief                   Saves the active smsc in the smsp file to a given record id.
 * @param uicc              Object to operate on.
 * @param client_tag        The client tag passed from client, client tag that will be returned in the callback function.
 * @param record_id         Record id that will be used to save the smsc.
 *
 * @return                  Status of the operation
 *
 */
uicc_request_status_t ste_uicc_sim_smsc_save_to_record(ste_sim_t * uicc,
                                                       uintptr_t client_tag,
                                                       int record_id);

/** Response type used when the cause is the STE_UICC_CAUSE_REQ_SMSC_RESTORE_FROM_RECORD.
 *
 *  @brief Data structure used for the data parameter in the ste_uicc_callback for the ste_uicc_sim_smsc_restore_from_record.
 *
 */
typedef struct {
  ste_uicc_status_code_t              uicc_status_code;              /**< uicc_status_code. Status */
  ste_uicc_status_code_fail_details_t uicc_status_code_fail_details; /**< uicc_status_code_fail_details. Status details */
  ste_uicc_status_word_t              status_word;                   /**< status_word. The status word retrieved. */
} ste_uicc_sim_smsc_restore_from_record_response_t;

/** Restores the smsc paramaters previously saved in the record identified by the record_id to the active smsc record. If
 *  a ste_uicc_sim_smsc_set_active call has been done prior to this call, the manufacturer SMSC default parameters,
 *  if set, will be stored in record 2 and can be restored to the active record from there.
 *
 * @brief                   Restores the smsc paramaters from a record to the active one.
 * @param uicc              Object to operate on.
 * @param client_tag        The client tag passed from client, client tag that will be returned in the callback function.
 * @param record_id         The record_id in the EFsmsp file where the smsc was previously stored.
 *
 * @return                  Status of the operation
 *
 */
uicc_request_status_t ste_uicc_sim_smsc_restore_from_record(ste_sim_t * uicc,
                                                            uintptr_t client_tag,
                                                            int record_id);

/** Response type used when the cause is the STE_UICC_CAUSE_REQ_SMSC_GET_RECORD_MAX.
 *
 *  @brief Data structure used for the data parameter in the ste_uicc_callback for the ste_uicc_sim_smsc_get_record_max.
 *
 */
typedef struct {
  ste_uicc_status_code_t              uicc_status_code;              /**< uicc_status_code. Status */
  ste_uicc_status_code_fail_details_t uicc_status_code_fail_details; /**< uicc_status_code_fail_details. Status details */
  ste_uicc_status_word_t              status_word;                   /**< status_word. The status word retrieved. */
  int                                 max_record_id;                 /**< Number of records in the EFsmsp file */
} ste_uicc_sim_smsc_get_record_max_response_t;

/** Retrieves the max record that can be used when saving an smsc in the EFsmsp file.
 *
 * @brief                   Retrieves the max record_id that can be used for saving an smsc in the EFsmsp file.
 * @param uicc              Object to operate on.
 * @param client_tag        The client tag passed from client, client tag that will be returned in the callback function.
 *
 * @return                  Status of the operation
 *
 */
uicc_request_status_t ste_uicc_sim_smsc_get_record_max(ste_sim_t * uicc,
                                                       uintptr_t client_tag);

/**
 * @brief	           Sync call for getting sim state information.
 * @param sim              Object to operate on.
 * @param client_tag       The client tag passed from client, client tag that will be returned in the callback function.
 * @param sim_state        State of the sim.
 * @return                 Status of the operation.
 */
int ste_uicc_sim_get_state_sync(ste_sim_t * sim,
                                uintptr_t client_tag,
                                ste_uicc_get_sim_state_response_t * sim_state);

/**
 * @brief	           Generic function call for reading from an EF file.
 * @param uicc             Object to operate on.
 * @param client_tag       The client tag passed from client, client tag that will be returned in the callback function.
 * @param file_id          file id to be operated on.
 * @param file_param       optional param, if not needed, put null for this param, otherwise put the data related to the read operation.
 * @param param_len        the length of the file param.
 * @return                 Status of the operation.
 */
uicc_request_status_t ste_uicc_sim_file_read_generic(ste_sim_t *uicc,
                                                     uintptr_t client_tag,
                                                     int file_id,
                                                     void * file_param,
                                                     int param_len);

/**
 * @brief	           Async call for reading the imsi file.
 * @param uicc             Object to operate on.
 * @param client_tag       The client tag passed from client, client tag that will be returned in the callback function.
 * @return                 Status of the operation.
 */
uicc_request_status_t ste_uicc_sim_file_read_imsi ( ste_sim_t * uicc,
                                                    uintptr_t client_tag );

/**
 * @brief                  Async call for reading the PLMN (6F30 or 6F60) files.
 * @param uicc             Object to operate on.
 * @param client_tag       The client tag passed from client, client tag that will be returned in the callback function.
 * @param file_id          PLMN file to read.
 * @return                 Status of the operation.
 */
uicc_request_status_t ste_uicc_sim_file_read_plmn(ste_sim_t *uicc,
                                                  uintptr_t  client_tag,
                                                  ste_uicc_sim_plmn_file_id_t file_id);

/**
 * @brief                  Async call for writing to the PLMN files.
 * @param uicc             Object to operate on.
 * @param client_tag       The client tag passed from client, client tag that will be returned in the callback function.
 * @param index            Index of the entry to write. First index is 0. If this is -1 then the first free entry should be used.
 * @param plmn_p           Data to write. If this is NULL then the existing entry with index index should be erased.
 * @param file_id          File to write plmn data to. If file_id is SIM_FILE_ID_PLMN_SEL, the AcT part of plmn is ignored.
 * @return                 Status of the operation.
 */
uicc_request_status_t ste_uicc_sim_file_update_plmn(ste_sim_t *uicc,
                                                    uintptr_t client_tag,
                                                    int index,
                                                    ste_sim_plmn_with_AcT_t *plmn_p,
                                                    ste_uicc_sim_plmn_file_id_t file_id);

/**
 * @brief Service type code used to specify service in service table.
 *
 */
typedef enum
{
  SIM_SERVICE_TYPE_LOCAL_PHONE_BOOK,
  SIM_SERVICE_TYPE_FDN,                       // Fixed Dialling Number
  SIM_SERVICE_TYPE_EXT_2,                     // Extension 2
  SIM_SERVICE_TYPE_SDN,                       // Service Dialling Number
  SIM_SERVICE_TYPE_EXT_3,                     // Extension 3
  SIM_SERVICE_TYPE_BDN,                       // Barred Dialling Number
  SIM_SERVICE_TYPE_EXT_4,                     // Extension 4
  SIM_SERVICE_TYPE_OCI_AND_OCT,               // Outgoing Call Information (OCI and OCT)
  SIM_SERVICE_TYPE_ICI_AND_ICT,               // Incoming Call Information (ICI and ICT)
  SIM_SERVICE_TYPE_SMS,                       // Short Message Storage
  SIM_SERVICE_TYPE_SMSR,                      // Short Message Service Reports
  SIM_SERVICE_TYPE_SMSP,                      // Short Message Service Parameters
  SIM_SERVICE_TYPE_AOC,                       // Advice of Charge
  SIM_SERVICE_TYPE_CCP,                       // Capability Configuration Parameters
  SIM_SERVICE_TYPE_CBMI,                      // Cell Broadcast Message Identifier
  SIM_SERVICE_TYPE_CBMIR,                     // Cell Broadcast Message Identifier Ranges
  SIM_SERVICE_TYPE_GID1,                      // Group Identifier Level 1
  SIM_SERVICE_TYPE_GID2,                      // Group Identifier Level 2
  SIM_SERVICE_TYPE_SPN,                       // Service Provider Name
  SIM_SERVICE_TYPE_PLMNWACT,                  // User controlled PLMN selector with Access Technology
  SIM_SERVICE_TYPE_MSISDN,                    // MSISDN
  SIM_SERVICE_TYPE_IMG,                       // Image
  SIM_SERVICE_TYPE_SOLSA,                     // Support of Localised Service Areas
  SIM_SERVICE_TYPE_ENH_PRECEDENCE_PREEMPTION, // Enhanced Multi-Level Precedence and Pre-emption Service
  SIM_SERVICE_TYPE_AAEM,                      // Automatic Answer for eMLPP
  SIM_SERVICE_TYPE_GSM_ACCESS,                // GSM Access
  SIM_SERVICE_TYPE_SMS_PP,                    // Data download via SMS-PP
  SIM_SERVICE_TYPE_SMS_CB,                    // Data download via SMS-CB
  SIM_SERVICE_TYPE_CALL_CONTROL_BY_USIM,      // Call Control by USIM
  SIM_SERVICE_TYPE_MO_SMS_CONTROL_BY_USIM,    // MO-SMS Control by USIM
  SIM_SERVICE_TYPE_RUN_AT_COMMAND,            // RUN AT COMMAND command
  SIM_SERVICE_TYPE_EST,                       // Enabled Services Table
  SIM_SERVICE_TYPE_ACL,                       // APN Control List
  SIM_SERVICE_TYPE_DCK,                       // Depersonalisation Control Keys
  SIM_SERVICE_TYPE_CNL,                       // Co-operative Network List
  SIM_SERVICE_TYPE_GSM_SECURITY_CONTEXT,      // GSM security context
  SIM_SERVICE_TYPE_CPBCCH,                    // CPBCCH Information
  SIM_SERVICE_TYPE_INV_SCAN,                  // Investigation Scan
  SIM_SERVICE_TYPE_MEXE,                      // MExE Service table
  SIM_SERVICE_TYPE_OPLMNWACT,                 // Operator controlled PLMN selector with Access Technology
  SIM_SERVICE_TYPE_HPLMNWACT,                 // HPLMN selector with Access Technology
  SIM_SERVICE_TYPE_EXT_5,                     // Extension 5
  SIM_SERVICE_TYPE_PNN,                       // PLMN Network Name
  SIM_SERVICE_TYPE_OPL,                       // Operator PLMN List
  SIM_SERVICE_TYPE_MBDN,                      // Mailbox Dialling Numbers
  SIM_SERVICE_TYPE_MWIS,                      // Message Waiting Indication Status
  SIM_SERVICE_TYPE_CFIS,                      // Call Forwarding Indication Status
  SIM_SERVICE_TYPE_SPDI,                      // Service Provider Display Information
  SIM_SERVICE_TYPE_MMS,                       // Multimedia Messaging Service
  SIM_SERVICE_TYPE_EXT_8,                     // Extension 8
  SIM_SERVICE_TYPE_CALL_CONTROL_ON_GPRS,      // Call control on GPRS by USIM
  SIM_SERVICE_TYPE_MMSUCP,                    // MMS User Connectivity Parameters
  SIM_SERVICE_TYPE_NIA,                       // Network's indication of alerting in the MS
  SIM_SERVICE_TYPE_VGCS_AND_VGCSS,            // VGCS Group Identifier List
  SIM_SERVICE_TYPE_VBS_AND_VBSS,              // VBS Group Identifier List
  SIM_SERVICE_TYPE_PSEUDO,                    // Pseudonym
  SIM_SERVICE_TYPE_UPLMNWLAN,                 // User Controlled PLMN selector for WLAN access
  SIM_SERVICE_TYPE_OPLMNWLAN,                 // Operator Controlled PLMN selector for WLAN access
  SIM_SERVICE_TYPE_USER_CONTR_WSID_LIST,      // User controlled WSID list
  SIM_SERVICE_TYPE_OPERATOR_CONTR_WSID_LIST,  // Operator controlled WSID list
  SIM_SERVICE_TYPE_VGCS_SECURITY,             // VGCS security
  SIM_SERVICE_TYPE_VBS_SECURITY,              // VBS security
  SIM_SERVICE_TYPE_WRI,                       // WLAN Reauthentication Identity
  SIM_SERVICE_TYPE_MMS_STORAGE,               // Multimedia Messages Storage
  SIM_SERVICE_TYPE_GBA,                       // Generic Bootstrapping Architecture
  SIM_SERVICE_TYPE_MBMS_SECURITY,             // MBMS security
  SIM_SERVICE_TYPE_DDL_USSD_APPL_MODE,        // Data download via USSD and USSD application mode
  SIM_SERVICE_TYPE_ADD_TERM_PROF,             // Additional TERMINAL PROFILE after UICC activation
  SIM_SERVICE_TYPE_CHV1_DISABLE,              // CHV1 disable function
  SIM_SERVICE_TYPE_ADN,                       // Abbreviated Dialling Numbers
  SIM_SERVICE_TYPE_PLMNSEL,                   // PLMN selector
  SIM_SERVICE_TYPE_EXT_1,                     // Extension 1
  SIM_SERVICE_TYPE_LND,                       // Last Number Dialled
  SIM_SERVICE_TYPE_MENU_SELECT,               // Menu selection
  SIM_SERVICE_TYPE_CALL_CONTROL,              // Call control
  SIM_SERVICE_TYPE_PROACTIVE_SIM,             // Proactive SIM
  SIM_SERVICE_TYPE_MO_SMS_CONTR_BY_SIM,       // Mobile Originated Short Message control by SIM
  SIM_SERVICE_TYPE_GPRS,                      // GPRS
  SIM_SERVICE_TYPE_USSD_STR_DATA_SUPPORT,     // USSD string data object supported in Call Control
  SIM_SERVICE_TYPE_ECCP,                       // Extended Capability Configuration Parameters
  SIM_SERVICE_TYPE_UNKNOWN
} sim_service_type_t;



/**
 * @brief                       async call for enable/disable a service by udpating the service table.
 * @param uicc                  Object to operate on.
 * @param client_tag            The client tag passed from client, client tag that will be returned in the callback function.
 * @param pin                   PIN code (PIN2) used to gain update rights of the service table.
 * @param service_type          service type to be enabled/disabled. The number of service types in this function are restricted.
 * @param enable_service        boolean: 1 = enable service, 0 = disable service .
 * @return                      Status of the operation.
 *
 * For USIM, only the following service types can be enabled/disabled: FDN, BDN and ACL.
 * For SIM, only the following service types can be enabled/disabled: FDN.
 * For all other service types the function will return uicc_status_code_fail_details == STE_UICC_STATUS_CODE_FAIL_DETAILS_SERVICE_NOT_SUPPORTED
 *
 */
uicc_request_status_t ste_uicc_update_service_table ( ste_sim_t * uicc,
                                                      uintptr_t client_tag,
                                                      const char *pin,
                                                      sim_service_type_t service_type,
                                                      uint8_t enable_service);


/**
 * @brief                       async call for retrieving the status of a service in the service table.
 * @param uicc                  Object to operate on.
 * @param client_tag            The client tag passed from client, client tag that will be returned in the callback function.
 * @param service_type          service type. The number of service types in this function are restricted.
 * @return                      Status of the operation.
 *
 * For USIM, only the following service types can be enabled/disabled: FDN, BDN and ACL.
 * For SIM, only the following service types can be enabled/disabled: FDN.
 * For all other service types the function will return uicc_status_code_fail_details == STE_UICC_STATUS_CODE_FAIL_DETAILS_SERVICE_NOT_SUPPORTED
 *
 */
uicc_request_status_t ste_uicc_get_service_table ( ste_sim_t * uicc,
                                                      uintptr_t client_tag,
                                                      sim_service_type_t service_type);

/**
 * @brief                       sync call for retrieving the status of a service in the service table.
 * @param sim                   Object to operate on.
 * @param client_tag            The client tag passed from client, client tag that will be returned in the callback function.
 * @param service_type          service type. The number of service types in this function are restricted.
 * @param read_response         structure to hold the read response.
 * @return                      Status of the operation.
 *
 * For USIM, only the following service types can be enabled/disabled: FDN, BDN and ACL.
 * For SIM, only the following service types can be enabled/disabled: FDN.
 * For all other service types the function will return uicc_status_code_fail_details == STE_UICC_STATUS_CODE_FAIL_DETAILS_SERVICE_NOT_SUPPORTED
 *
 */
int ste_uicc_get_service_table_sync ( ste_sim_t * sim,
                                      uintptr_t client_tag,
                                      sim_service_type_t service_type,
                                      ste_uicc_get_service_table_response_t * read_response);


/**
 * @brief                       async call for retrieving the availability of a service in the service table.
 * @param uicc                  Object to operate on.
 * @param client_tag            The client tag passed from client, client tag that will be returned in the callback function.
 * @param service_type          service type .
 * @return                      Status of the operation.
 *
 */
uicc_request_status_t ste_uicc_get_service_availability ( ste_sim_t * uicc,
                                                          uintptr_t client_tag,
                                                          sim_service_type_t service_type);

/**
 * @brief                       sync call for retrieving the availability of a service in the service table.
 * @param sim                   Object to operate on.
 * @param client_tag            The client tag passed from client, client tag that will be returned in the callback function.
 * @param service_type          service type.
 * @param read_response         structure to hold the read response.
 * @return                      Status of the operation.
 *
 */
int ste_uicc_get_service_availability_sync ( ste_sim_t * sim,
                                             uintptr_t client_tag,
                                             sim_service_type_t service_type,
                                             ste_uicc_get_service_availability_response_t * read_response);


/**
 * @brief	           Gets icon format, or icon image data.
 * @param uicc             Object to operate on.
 * @param client_tag       The client tag passed from client, client tag that will be returned in the callback function.
 * @param file_id          file id to be operated on.
 * @param record_or_offset Record nbr or offset into the file
 * @param length           Max length of data to be read out
 * @return                 Status of the operation.
 *
 * If the given file id is EF-IMG, then the record with id number
 * record_or_offset is read from that EF. If length is zero,
 * then the length of the image data is returned.
 * If the file id is an EF-IID, then image data is read.
 *
 * Reading out image data using this method is a two step process. The first
 * step gets image format by reading from the EF-IMG, and next step gives the
 * contents from particular EF-IID file id obtained from the first step.
 *
 * If there is an error, then the length is 0, data is NULL, and type is set
 * to STE_UICC_READ_ICON_NULL.
 */
uicc_request_status_t ste_uicc_sim_icon_read(ste_sim_t *uicc,
                                             uintptr_t client_tag,
                                             int file_id,
                                             int record_or_offset,
                                             int length);

/**
 * @brief Data structure used for the data parameter in the ste_uicc_callback
 *              when the cause is STE_UICC_CAUSE_REQ_SIM_ICON_READ;
 */
typedef struct {
  ste_uicc_status_code_t uicc_status_code; /**< uicc_status_code. Status */
  ste_uicc_status_code_fail_details_t uicc_status_code_fail_details; /**< uicc_status_code_fail_details. Status details */
  ste_uicc_status_word_t status_word; /**< status_word. The status word retrieved. */
  uint16_t length;                 /**< The length of data returned */
  uint8_t *data;                   /**< data. data retrieved. */
} ste_uicc_sim_icon_read_response_t;

/**
 * @brief              Gets the MSISDNs related to the subscriber.
 * @param uicc         Object to operate on.
 * @param client_tag   The client tag passed from client, client tag that will be returned in the callback function.
 * @return             Status of the operation.
 *
 * Reads out the content from records in the EF-MSISDN file.
 */
uicc_request_status_t ste_uicc_get_subscriber_number(ste_sim_t *uicc,
        uintptr_t client_tag);

/**
 * @brief Subscriber number record struct with params alpha id, actual number, type, speed, service and itc.
 */

typedef struct {
    uint8_t  alpha_id_actual_len;   /**< Actual length of alpha identifier. */
    uint8_t  alpha_coding;          /**< Coding scheme of alpha identifier. */
    uint8_t  *alpha_p;              /**< Actual alpha identifier. */
    uint8_t  number_actual_len;     /**< Length of the subscriber number. */
    uint8_t  *number_p;             /**< The subscriber number. */
    uint8_t  type;                  /**< Type */
    uint8_t  speed;                 /**< Speed */
    uint8_t  service;               /**< Current service. */
    uint8_t  itc;                   /**< Information transfer capability */
} ste_sim_subscriber_number_record_t;

/**
 * @brief Data structure used for the data parameter in the ste_uicc_callback
 *        when the cause is STE_UICC_CAUSE_REQ_READ_SUBSCRIBER_NUMBER;
 */
typedef struct {
    ste_uicc_status_code_t                uicc_status_code; /**< uicc_status_code. Status */
    ste_uicc_status_code_fail_details_t   uicc_status_code_fail_details; /**< uicc_status_code_fail_details. Status details */
    ste_uicc_status_word_t                status_word; /**< status_word. The status word retrieved.*/
    uint8_t                               alpha_id_len; /**< The length of the Alpha Identifier. */
    uint16_t                              number_of_records; /**< Number of records from the MSISDN-file. */
    uint16_t                              record_length; /**< The length of data for one record. */
    ste_sim_subscriber_number_record_t    *record_data_p; /**< Pointer to the MSISDN-records. */
} ste_sim_subscriber_number_response_t;
/**
 * @brief                  Get information about application on uicc
 * @param uicc             Object to operate on.
 * @param client_tag       The client tag passed from client, client tag that will be returned in the callback function.
 * @param app_index        Application index, from 0 to num_apps-1 as
 *                         returned from ste_uicc_card_status
 *
 * @return                 status of the call
 *
 * A successful call returns UICC_REQUEST_STATUS_OK. The call back has
 * cause STE_UICC_CAUSE_REQ_APP_STATUS and response data structure of the
 * type ste_uicc_sim_app_status_response_t. (see below).
 * For a failed call, there is no call back.
 *
 * The app_index always refers to the same application.
 */

uicc_request_status_t ste_uicc_app_status(ste_sim_t * uicc,
                                           uintptr_t client_tag,
                                           int app_index);
/**
 * @brief Data structures used for the data parameter in the ste_uicc_callback
 *        when the cause is STE_UICC_CAUSE_REQ_APP_STATUS
 */

/**
 * @brief Enum used to mark sim applications for special use
 */
typedef enum {
  SIM_APP_MARK_UNKNOWN,   /* Unknown mark */
  SIM_APP_MARK_NONE,      /* Not recognized as special */
  SIM_APP_MARK_GSM_UMTS,  /* Selected gsm or umts application */
  SIM_APP_MARK_CDMA       /* Selected cdma application */
} sim_app_mark_t;

/**
 * @brief Enum used to show current application lifecycle state
 *
 * The initial application state is STE_UICC_APP_STATE_NOT_ACTIVE
 * Then it transits to STE_UICC_APP_STATE_ACTIVE_PIN/PUK if the application
 * needs PIN/PUK in order to execute, and finally
 * STE_UICC_APP_STATE_ACTIVE when it is operating nominally. If the application
 * closes down, it goes back to STE_UICC_APP_STATE_NOT_ACTIVE.
 *
 */
typedef enum {
  STE_UICC_APP_STATE_UNKNOWN,     /* State has not yet been determined */
  STE_UICC_APP_STATE_ACTIVE,      /* Application is selected and activated */
  STE_UICC_APP_STATE_NOT_ACTIVE,  /* Application is not active */
  STE_UICC_APP_STATE_ACTIVE_PIN,  /* Active application needs PIN verify */
  STE_UICC_APP_STATE_ACTIVE_PUK   /* Active application needs PUK verify */
} ste_uicc_app_state_t;

/**
 * @brief Enum used to indicate if application PIN is replaced with UPIN
 */
typedef enum {
  SIM_APP_PIN_MODE_NORMAL,   /* Application has PIN as application PIN  */
  SIM_APP_PIN_MODE_REPLACED  /* Application has UPIN as application PIN */
} sim_app_pin_mode_t;

/**
 * @brief Enum used to show current state of PIN
 */
typedef enum {
  STE_UICC_PIN_STATE_UNKNOWN, /* State unknown or not yet determined */
  STE_UICC_PIN_STATE_ENABLED,   /* PIN is enabled */
  STE_UICC_PIN_STATE_DISABLED,  /* PIN is disabled */
  STE_UICC_PIN_STATE_ENABLED_NOT_VERIFIED, /* Verify is needed, but not done */
  STE_UICC_PIN_STATE_ENABLED_VERIFIED, /* Verify has been done */
  STE_UICC_PIN_STATE_ENABLED_BLOCKED,  /* PIN can be unblocked and verified with PUK */
  STE_UICC_PIN_STATE_ENABLED_PERM_BLOCKED /* PIN can never be unblocked or verified */
} ste_uicc_pin_state_t;

#define MAX_AID_LEN (16) /* Max length of aid field */

/**
 * @brief Structure for data describing an application returned with ste_uicc_app_status
 *
 * For a successful request the status field is set to STE_UICC_STATUS_CODE_OK
 * and data can be retrieved from the remaining fields of the structure.
 * The request fails with a status value of STE_UICC_STATUS_CODE_FAIL if the
 * app_index is out of range, and then the remaining fields of the structure
 * have undefined values.
 *
 * Repeated calls to ste_uicc_app_status for a given app_index return new
 * instances of this structure where some values may have changed. The following
 * fields are constant: app_type, aid_len, aid, label_len,
 * label, mark. app_state, pin_mode, pin_state, pin2_state may change values
 * and when this happens, sim_status event is generated to notify the client.
 */
typedef struct {
  ste_uicc_status_code_t status;            /**< Current UICC status. */
  sim_app_type_t         app_type;          /**< Application type. */
  ste_uicc_app_state_t   app_state;         /**< Application state. */
  size_t                 aid_len;           /**< Actual aid length, less or equal to MAX_AID_LEN. */
  uint8_t                aid[MAX_AID_LEN];  /**< Current aid. */
  size_t                 label_len;         /**< Application label size. */
  char                  *label;             /**< Application label. */
  sim_app_mark_t         mark;              /**< Marker for special application use. */
  sim_app_pin_mode_t     pin_mode;          /**< Current pin mode. */
  ste_uicc_pin_state_t   pin_state;         /**< Current pin state. */
  ste_uicc_pin_state_t   pin2_state;        /**< Current pin2 state. */
} ste_uicc_sim_app_status_response_t;

/**
 * @brief                  Get information about the uicc card
 * @param uicc             Object to operate on.
 * @param client_tag       The client tag passed from client, client tag that will be returned in the callback function.
 */

uicc_request_status_t ste_uicc_card_status(ste_sim_t * uicc,
                                           uintptr_t client_tag);

/**
 * @brief Data structure used for the data parameter in the ste_uicc_callback
 *             when the cause is STE_UICC_CAUSE_REQ_CARD_STATUS;
 */
#define UICC_MAX_APPS            (8)

typedef enum {
  SIM_CARD_TYPE_UNKNOWN,
  SIM_CARD_TYPE_ICC,
  SIM_CARD_TYPE_UICC,
  SIM_CARD_TYPE_USB
} sim_card_type_t;

typedef enum {
  STE_UICC_CARD_STATE_UNKNOWN,
  STE_UICC_CARD_STATE_READY,
  STE_UICC_CARD_STATE_NOT_READY,
  STE_UICC_CARD_STATE_MISSING,
  STE_UICC_CARD_STATE_INVALID,
  STE_UICC_CARD_STATE_DISCONNECTED
} ste_uicc_card_state_t;

/**
 * @brief card status response struct with params status, number of apps, card type, card state and upin state.
 */

typedef struct {
  ste_uicc_status_code_t status;    /**< Command status, OK or FAIL. */
  int num_apps;                     /**< Number of apps on card. */
  sim_card_type_t card_type;        /**< Card type. */
  ste_uicc_card_state_t card_state; /**< Current card state. */
  ste_uicc_pin_state_t upin_state;  /**< Current upin state. */
} ste_uicc_sim_card_status_response_t;

/**
 * @brief                  Opens a logical channel to a UICC application
 * @param uicc             Object to operate on.
 * @param client_tag       The client tag passed from client, client tag that will be returned in the callback function.
 * @param aid_len          The length of the AID
 * @param aid              The Application ID (AID) of the UICC application.
 * @return                 Status of the operation.
 *
 * If successful, this returns a channel session ID that can be used
 * with the channel send request and channel close request.
 * Also, the application on the UICC connected to this channel
 * will have been activated.
 *
 * @note
 * The current implementation is restricted to non-telecom applications.
 * If the telecom AID is used, the request will fail.
 * If the telecom AID is NULL, the request will fail.
 */
uicc_request_status_t ste_uicc_sim_channel_open(ste_sim_t       *uicc,
                                                uintptr_t       client_tag,
                                                uint32_t        aid_len,
                                                uint8_t         *aid);
/**
 * @brief channel open response struct with parameters: uicc status code, uicc status code fail details,
 * status word and session id.
 */

typedef struct {
    ste_uicc_status_code_t              uicc_status_code;              /**< uicc_status_code. Status */
    ste_uicc_status_code_fail_details_t uicc_status_code_fail_details; /**< uicc_status_code_fail_details. Status details */
    ste_uicc_status_word_t              status_word;                   /**< status_word. The status word retrieved. */
    uint16_t                            session_id;                    /**< the created session ID */
} ste_uicc_sim_channel_open_response_t;

/**
 * @brief                  Closes a logical channel to a UICC application
 * @param uicc             Object to operate on.
 * @param client_tag       The client tag passed from client, client tag that will be returned in the callback function.
 * @param session_id       The channel session ID from an open channel request.
 * @return                 Status of the operation.
 *
 * If successful, the UICC application will be terminated and the
 * logical channel will be closed.
 */
uicc_request_status_t ste_uicc_sim_channel_close(ste_sim_t      *uicc,
                                                 uintptr_t      client_tag,
                                                 uint16_t       session_id);
/**
 * @brief Channel close response struct with paramters: uicc status code, uicc status code fail details and status word.
 */

typedef struct {
    ste_uicc_status_code_t              uicc_status_code;              /**< uicc_status_code. Status */
    ste_uicc_status_code_fail_details_t uicc_status_code_fail_details; /**< uicc_status_code_fail_details. Status details */
    ste_uicc_status_word_t              status_word;                   /**< status_word. The status word retrieved. */
} ste_uicc_sim_channel_close_response_t;

/**
 * @brief                  Send a message on a logical channel to a UICC application
 * @param uicc             Object to operate on.
 * @param client_tag       The client tag passed from client, client tag that will be returned in the callback function.
 * @param session_id       The channel session ID from an open channel request.
 * @param apdu_len         The length of the APDU to send.
 * @param apdu             The APDU data
 * @return                 Status of the operation.
 */
uicc_request_status_t ste_uicc_sim_channel_send(ste_sim_t   *uicc,
                                                uintptr_t   client_tag,
                                                uint16_t    session_id,
                                                uint32_t    apdu_len,
                                                uint8_t     *apdu);
/**
 * @brief Channel send response struct with paramters: uicc status code, uicc status code fail details,
 * status word, APDU length and actual APDU.
 */

typedef struct {
    ste_uicc_status_code_t              uicc_status_code;              /**< uicc_status_code. Status */
    ste_uicc_status_code_fail_details_t uicc_status_code_fail_details; /**< uicc_status_code_fail_details. Status details */
    ste_uicc_status_word_t              status_word;                   /**< status_word. The status word retrieved. */
    size_t                              apdu_len;                      /**< The length of the send response */
    uint8_t                             *apdu;                         /**< The send response */
} ste_uicc_sim_channel_send_response_t;

/**
 * @brief FDN record struct with parameters: dial string, alpha string, ton and npi.
 */

typedef struct {
  ste_sim_text_t              dial_string; /**< Dial string, complete with number and supplementary Services Control characters */
  ste_sim_text_t              alpha_string;       /**< Alpha identifier */
  ste_sim_type_of_number_t    ton;         /**< Type Of Network (TON) */
  ste_sim_numbering_plan_id_t npi;         /**< Number Plan Identification (NPI) */
} ste_uicc_sim_fdn_record_t;

/**
 * @brief Data structure used for the data parameter in the ste_uicc_callback
 *        when the cause is STE_UICC_CAUSE_REQ_SIM_FILE_READ_FDN;
 */

typedef struct {
    ste_uicc_status_code_t              uicc_status_code;  /**< uicc_status_code. Status */
    ste_uicc_status_code_fail_details_t uicc_status_code_fail_details; /**< uicc_status_code_fail_details. Status details */
    ste_uicc_status_word_t              status_word;       /**< status_word. The status word retrieved.*/
    size_t                              number_of_records; /**< Number of records in fdn_record_p. */
    ste_uicc_sim_fdn_record_t          *fdn_record_p;      /**< Pointer to the FDN-records. */
} ste_uicc_sim_fdn_response_t;

/**
 * @brief              Read FDN
 * @param uicc         Object to operate on.
 * @param client_tag   The client tag passed from client, client tag that will be returned in the callback function.
 * @return             Status of the operation.
 *
 * Reads out contents of the EF-FDN file.
 */

uicc_request_status_t ste_uicc_sim_file_read_fdn(ste_sim_t *uicc,
        uintptr_t client_tag);

/**
 * @brief              Read FDN
 * @param uicc         Object to operate on.
 * @param client_tag   The client tag passed from client, client tag that will be returned in the callback function.
 * @param result       structure to hold the read result
 * @return             Status of the operation
 *
 * Reads out contents of the EF-FDN file.
 * NOTE: Client needs to free data in fdn_record_p pointer.
 */

int ste_uicc_sim_file_read_fdn_sync(ste_sim_t *uicc,
                                                 uintptr_t client_tag,
                                                 ste_uicc_sim_fdn_response_t *result);
/**
 * Constants from in 3GPP TS 24.008 to be used by client to mask out
 * values from the category byte of ste_uicc_sim_ecc_number_t
 */

typedef enum {
    STE_UICC_ECC_CATEGORY_POLICE            = (1<<0),
    STE_UICC_ECC_CATEGORY_AMBULANCE         = (1<<1),
    STE_UICC_ECC_CATEGORY_FIRE_BRIGADE      = (1<<2),
    STE_UICC_ECC_CATEGORY_MARINE_GUARD      = (1<<3),
    STE_UICC_ECC_CATEGORY_MOUNTAIN_RESCUE   = (1<<4),
    STE_UICC_ECC_CATEGORY_MANUAL_ECALL      = (1<<5),
    STE_UICC_ECC_CATEGORY_AUTO_ECALL        = (1<<6),
    STE_UICC_ECC_CATEGORY_RESERVED          = (1<<7),
    STE_UICC_ECC_CATEGORY_UNUSED            = 0
} ste_uicc_ecc_category_t;

/**
 * @brief Structure for ECC data
 *
 * Note:
 * GSM only defines the first field (the number to dial).  In this
 * case, the remaining elements are all set to zero.
 *
 * Coding of the alpha identifier:
 * - the SMS default 7-bit coded alphabet as defined in TS 23.038
 * - one of the UCS2 coded options as defined in the annex of TS 31.101
 */
typedef struct {
    char                     number[7];  /**< Number string, max 6 chars */
    ste_uicc_ecc_category_t  category;   /**< raw ecc category byte (as read from EF ECC) */
    size_t                   length;     /**< number of bytes in the alpha identifier. */
    uint8_t*                 alpha;      /**< raw alpha identifier (as read from EF ECC) */
} ste_uicc_sim_ecc_number_t;

/**
 * @brief Structure for ECC data returned with ste_uicc_sim_file_read_ecc
 *
 * NOTE: Status word originate from reading the file. The case when the status
 * word indicate the file cannot be found is handled as a successful case.
 * NOTE: If the card has no ECC numbers stored, the function returns zero
 * entries and is considered a successful case.
 */
typedef struct {
    ste_uicc_status_code_t              uicc_status_code;               /**< uicc_status_code. Status */
    ste_uicc_status_code_fail_details_t uicc_status_code_fail_details;  /**< uicc_status_code_fail_details. Status details */
    ste_uicc_status_word_t              status_word;                    /**< status_word. The status word retrieved. */
    size_t                              number_of_records;              /**< number of records */
    ste_uicc_sim_ecc_number_t          *ecc_number_p;                   /**< pointer to ECC records */
} ste_uicc_sim_ecc_response_t;

/**
 * @brief                  Async call for reading the ECC file.
 * @param uicc             Object to operate on.
 * @param client_tag       The client tag passed from client, client tag that will be returned in the callback function.
 * @return                 Status of the operation.
 */
uicc_request_status_t ste_uicc_sim_file_read_ecc(ste_sim_t *uicc,
                                                 uintptr_t client_tag);

/**
 * @brief                  Sync call for reading the ECC file.
 * @param uicc             Object to operate on.
 * @param client_tag       The client tag passed from client, client tag that will be returned in the callback function.
 * @param result           Pointer to where the result should be stored.
 * @return                 Status of the operation.
 */
int ste_uicc_sim_file_read_ecc_sync(ste_sim_t *uicc,
                                    uintptr_t client_tag,
                                    ste_uicc_sim_ecc_response_t *result);

/**
 * @brief Data structure used for the data parameter in the ste_uicc_callback
 *        when the cause is STE_UICC_CAUSE_REQ_SIM_RESET;
 */
typedef struct {
    ste_uicc_status_code_t              uicc_status_code;  /**< uicc_status_code. Status */
    ste_uicc_status_code_fail_details_t uicc_status_code_fail_details; /**< uicc_status_code_fail_details. Status details */
    ste_uicc_status_word_t              status_word;       /**< status_word. The status word retrieved.*/
} ste_uicc_sim_reset_response_t;

/*!
 * Used for SIM REFRESH RESET testing purposes only. Resets SIMD to init state.
 * @param[in] uicc Object to operate on.
 * @param[in] client_tag The client tag passed from client, client tag that will be returned in the callback function.
 * @return Status of the operation.
 */
int ste_uicc_sim_reset(ste_sim_t *uicc, uintptr_t client_tag);

/**
 * @brief SIM card power on response struct.
 */

typedef struct {
    ste_uicc_status_code_t              uicc_status_code;              /**< uicc_status_code. Status */
    ste_uicc_status_code_fail_details_t uicc_status_code_fail_details; /**< uicc_status_code_fail_details. Status details */
    ste_uicc_status_word_t              status_word;                   /**< status_word. The status word retrieved. */
} ste_uicc_sim_power_on_response_t;

/**
 * @brief                  Power on the sim card
 * @param uicc             Object to operate on.
 * @param client_tag       The client tag passed from client, client tag that will be returned in the callback function.
 *
 * @return                 status of the function call
 *
 * A successful call returns UICC_REQUEST_STATUS_OK. The call back has
 * cause STE_UICC_CAUSE_REQ_SIM_POWER_ON and response data structure of the
 * type ste_uicc_sim_power_on_response_t.
 * This interface does not need to be called if the card has not been explicitly powered down by the power off interface
 * For a failed call, there is no call back.
 *
 */

uicc_request_status_t ste_uicc_sim_power_on(ste_sim_t * uicc,
                                            uintptr_t client_tag);

/**
 * @brief SIM card power off response struct.
 */

typedef struct {
    ste_uicc_status_code_t              uicc_status_code;              /**< uicc_status_code. Status */
    ste_uicc_status_code_fail_details_t uicc_status_code_fail_details; /**< uicc_status_code_fail_details. Status details */
    ste_uicc_status_word_t              status_word;                   /**< status_word. The status word retrieved. */
} ste_uicc_sim_power_off_response_t;

/**
 * @brief                  Power off the sim card
 * @param uicc             Object to operate on.
 * @param client_tag       The client tag passed from client, client tag that will be returned in the callback function.
 *
 * @return                 status of the function call
 *
 * A successful call returns UICC_REQUEST_STATUS_OK. The call back has
 * cause STE_UICC_CAUSE_REQ_SIM_POWER_OFF and response data structure of the
 * type ste_uicc_sim_power_off_response_t.
 * The sim state will be SIM_STATE_UNKNOWN after this interface is called.
 * For a failed call, there is no call back.
 *
 */

uicc_request_status_t ste_uicc_sim_power_off(ste_sim_t * uicc,
                                             uintptr_t client_tag);

/**
 * @brief Type used when reading the preferred RAT (Radio Access Technology) mode settings from the SIM.
 */
typedef enum ste_uicc_sim_RAT_setting_tag {
  STE_UICC_SIM_RAT_SETTING_NO_PREFERENCES = 0, /**< The SIM has no preferred RAT setting */
  STE_UICC_SIM_RAT_SETTING_DUAL_MODE,          /**< SIM mandates that the RAT mode shall be set to dual mode */
  STE_UICC_SIM_RAT_SETTING_2G_ONLY,            /**< SIM mandates that the RAT mode shall be set to 2G only */
  STE_UICC_SIM_RAT_SETTING_3G_ONLY,            /**< SIM mandates that the RAT mode shall be set to 3G only */
  STE_UICC_SIM_RAT_SETTING_ENABLE_ALL_SUPPORTED_RATS /**< SIM mandates that all supported RAT shall be enabled */
} ste_uicc_sim_RAT_setting_t;

/** Response type used when reading for the STE_UICC_CAUSE_REQ_SIM_READ_RAT_MODE_SETTINGS
 *  response callback. Contains the SIM preferred RAT mode, if found on the SIM card.
 *
 */
typedef struct {
    ste_uicc_status_code_t              uicc_status_code;              /**< uicc_status_code. Status */
    ste_uicc_status_code_fail_details_t uicc_status_code_fail_details; /**< uicc_status_code_fail_details. Status details */
    ste_uicc_sim_RAT_setting_t          RAT;                           /**< The SIM preferred RAT setting */
} ste_uicc_sim_read_preferred_RAT_setting_response_t;

/**
 * @brief                  Reads the SIM preferred RAT mode setting if present on the SIM card.
 * @param uicc             Object to operate on.
 * @param client_tag       The client tag passed from client, client tag that will be returned in the callback function.
 *
 * @return                 status of the function call
 *
 * Some SIM cards have preferred RAT mode settings which shall override any user given
 * RAT setting if found on the SIM. The method is used for retrieving these settings
 * from the SIM card. A successful call returns UICC_REQUEST_STATUS_OK. The call back has
 * cause STE_UICC_CAUSE_REQ_SIM_READ_PREFERRED_RAT_SETTING and response data structure of the
 * type ste_uicc_sim_read_preferred_RAT_setting_response_t.
 *
 */

uicc_request_status_t ste_uicc_sim_read_preferred_RAT_setting(ste_sim_t * uicc,
                                                              uintptr_t client_tag);

/**
 * @brief       Send RAW apdu to modem
 * @param cat   Object to operate on.
 * @param client_tag The client tag that will be returned in the callback function.
 * @param buf The raw APDu to be sent to SIM.
 * @param len The length of the raw APDU data
 * @return      Status of the operation.  0 on success, -1 on failure.
 *
 */

int ste_cat_raw_apdu(ste_sim_t * cat, uintptr_t client_tag, const char *buf, unsigned len);

#endif
