/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Description: Chache handling of PBR data.
 *
 * Author:  Dag Gullberg <dag.xd.gullberg@stericsson.com>
 *          Alex Macro <alex.macro@stericsson.com>
 *          Mats Bengtsson <mats.m.bengtsson@stericsson.com>
 */


#ifndef _INCLUSION_GUARD_SIMPB_H_
#define _INCLUSION_GUARD_SIMPB_H_

#include <stdint.h>

#define PB_MAX_FILE_IDS 10

typedef enum {
    /* Values mapped from sim.h */
    STE_SIMPB_ISO_NORMAL_COMPLETION,
    STE_SIMPB_ISO_WARNING_62XX_NVM_UNCHANGED,
    STE_SIMPB_ISO_WARNING_62XX_POSSIBLE_INVALID_DATA,
    STE_SIMPB_ISO_WARNING_62XX_POSSIBLE_INVALID_FILE,
    STE_SIMPB_ISO_WARNING_63XX_NVM_CHANGED,
    STE_SIMPB_ISO_WARNING_63XX_VERIFICATION_FAILED,
    STE_SIMPB_ISO_ERROR_64XX_NVM_UNCHANGED,
    STE_SIMPB_ISO_ERROR_65XX_NVM_CHANGED,
    STE_SIMPB_ISO_ERROR_6581_MEMORY_FAILURE,
    STE_SIMPB_ISO_ERROR_66XX_SECURITY_RELATED,
    STE_SIMPB_ISO_ERROR_6700_WRONG_LENGTH,
    STE_SIMPB_ISO_ERROR_68XX_FUNCTION_NOT_SUPPORTED,
    STE_SIMPB_ISO_ERROR_69XX_COMMAND_NOT_ALLOWED,
    STE_SIMPB_ISO_ERROR_69XX_INVALID_SM_DATA_OBJECT,
    STE_SIMPB_ISO_ERROR_6982_SECURITY_NOT_SATISFIED,
    STE_SIMPB_ISO_ERROR_6983_PIN_BLOCKED,
    STE_SIMPB_ISO_ERROR_6984_DATA_INVALIDATED,
    STE_SIMPB_ISO_ERROR_6985_CONDITION_OF_USE,
    STE_SIMPB_ISO_ERROR_6AXX_WRONG_PARAMETERS,
    STE_SIMPB_ISO_ERROR_6A80_WRONG_PARAMETER_DATA,
    STE_SIMPB_ISO_ERROR_6A81_FUNCTION_NOT_SUPPORTED,
    STE_SIMPB_ISO_ERROR_6A82_FILE_NOT_FOUND,
    STE_SIMPB_ISO_ERROR_6A83_RECORD_NOT_FOUND,
    STE_SIMPB_ISO_ERROR_6A84_NO_MEMORY,
    STE_SIMPB_ISO_ERROR_6A86_WRONG_P1_P2,
    STE_SIMPB_ISO_ERROR_6A88_DATA_NOT_FOUND,
    STE_SIMPB_ISO_ERROR_6BXX_WRONG_PARAMETERS,
    STE_SIMPB_ISO_ERROR_6CXX_WRONG_LENGTH,
    STE_SIMPB_ISO_ERROR_6DXX_INS_NOT_SUPPORTED,
    STE_SIMPB_ISO_ERROR_6EXX_CLA_NOT_SUPPORTED,
    STE_SIMPB_ISO_ERROR_UNSPECIFIED,
    STE_SIMPB_ISO_WARNING_APPL_POSSIBLE_INVALID_DATA,
    STE_SIMPB_ISO_WARNING_APPL_GSM_AKA_PERFORMED,
    STE_SIMPB_ISO_ERROR_APPL_NO_ANSWER,
    STE_SIMPB_ISO_ERROR_APPL_NO_SERVICE,
    STE_SIMPB_ISO_ERROR_APPL_BUSY,
    STE_SIMPB_ISO_ERROR_APPL_IDLE,
    STE_SIMPB_ISO_ERROR_APPL_SEQUENCE_ERROR,
    STE_SIMPB_ISO_ERROR_APPL_PARAMETER_ERROR,
    STE_SIMPB_ISO_ERROR_APPL_DECODE_ERROR,
    STE_SIMPB_ISO_ERROR_APPL_PIN_BLOCKED,
    STE_SIMPB_ISO_ERROR_APPL_PIN_BLOCKED_FOREVER,
    STE_SIMPB_ISO_ERROR_APPL_WAIT_FOR_PIN,
    STE_SIMPB_ISO_ERROR_APPL_PIN_INVALID_FORMAT,
    STE_SIMPB_ISO_ERROR_APPL_OUT_OF_MEMORY,
    STE_SIMPB_ISO_ERROR_APPL_NO_SPACE_IN_FILE,
    STE_SIMPB_ISO_ERROR_APPL_AUTH_ERROR_MAC,
    STE_SIMPB_ISO_ERROR_APPL_AUTH_ERR_CONTEXT_NOT_SUPPORTED,
    STE_SIMPB_ISO_ERROR_APPL_GSM_AKA_NOT_ALLOWED,
    STE_SIMPB_ISO_ERROR_APPL_NO_INCREASE_MAX_VALUE_REACHED,
    STE_SIMPB_ISO_ERROR_APPL_KEY_FRESHNESS_FAILURE,
    STE_SIMPB_ISO_ERROR_APPL_UNSPECIFIED
} ste_simpb_iso_t;

typedef enum {
    /* Values mapped from sim.h */
    STE_SIMPB_SIM_STATE_UNKNOWN,                     /**< Unknown state of the SIM card. */
    STE_SIMPB_SIM_STATE_SIM_ABSENT,                  /**< SIM card is absent due to failure or detachment. */
    STE_SIMPB_SIM_STATE_NOT_READY,                   /**< SIM card is not ready to accept any requests from client. */
    STE_SIMPB_SIM_STATE_PIN_NEEDED,                  /**< SIM card requests for PIN1 verification. */
    STE_SIMPB_SIM_STATE_READY,                       /**< SIM card is ready to serve client requests. */
    STE_SIMPB_SIM_STATE_PIN2_NEEDED,                 /**< SIM card requests for PIN2 verification. */
    STE_SIMPB_SIM_STATE_PUK_NEEDED,                  /**< SIM card requests for PUK verification. */
    STE_SIMPB_SIM_STATE_PUK2_NEEDED,                 /**< SIM card requests for PUK2 verification. */
    STE_SIMPB_SIM_STATE_SAP,                         /**< SIM card is serving the Bluetooth SIM Access Profile. */
    STE_SIMPB_SIM_STATE_PERMANENTLY_BLOCKED
} ste_simpb_sim_state_t;

typedef enum {
    STE_SIMPB_SUCCESS,
    STE_SIMPB_PENDING,              /**< Only used internally by the daemon */
    STE_SIMPB_FAILURE,
    STE_SIMPB_NOT_SUPPORTED,        /**< Only used internally by the daemon */
    STE_SIMPB_ERROR_PARAM,
    STE_SIMPB_ERROR_PARAM_NULL,
    STE_SIMPB_ERROR_UNKNOWN,
    STE_SIMPB_ERROR_BAD_HANDLE,     /**< The ste_sim_t handle was NULL, or somehow broken */
    STE_SIMBP_ERROR_SOCKET,         /**< An error relating to use of sockets */
    STE_SIMBP_ERROR_NOT_CONNECTED,  /**< Not connected */
    STE_SIMBP_ERROR_SEQUENCE,       /**< Sequence of commands incorrect */
    STE_SIMPB_ERROR_RESOURCE        /**< Unable to allocate a resource - typically memory or threads */
} ste_simpb_result_t;

typedef enum {
    STE_SIMPB_STATUS_DISCONNECTED,
    STE_SIMPB_STATUS_CONNECTED,
    STE_SIMPB_STATUS_INITIALIZING,  /**< Transferring from CONNECTED to INITIALIZED */
    STE_SIMPB_STATUS_INITIALIZED,
    STE_SIMPB_STATUS_SHUTTING_DOWN, /**< Transferring from INITIALIZED to CONNECTED */
    STE_SIMPB_STATUS_NO_SIM,
} ste_simpb_status_t;

typedef enum {
    STE_SIMPB_PHONEBOOK_TYPE_INVALID = 0,
    STE_SIMPB_PHONEBOOK_TYPE_GLOBAL  = 1,
    STE_SIMPB_PHONEBOOK_TYPE_LOCAL   = 2,
    STE_SIMPB_PHONEBOOK_TYPE_DEFAULT = 128
} ste_simpb_phonebook_type_t;

typedef enum {
    STE_SIMPB_FILE_TYPE_PBR,  /* do not change the order */
    STE_SIMPB_FILE_TYPE_ADN,
    STE_SIMPB_FILE_TYPE_EXT1,
    STE_SIMPB_FILE_TYPE_CCP1,
    STE_SIMPB_FILE_TYPE_AAS,
    STE_SIMPB_FILE_TYPE_GAS,
    STE_SIMPB_FILE_TYPE_UID,
    STE_SIMPB_FILE_TYPE_IAP,
    STE_SIMPB_FILE_TYPE_GRP,
    STE_SIMPB_FILE_TYPE_ANR,
    STE_SIMPB_FILE_TYPE_EMAIL,
    STE_SIMPB_FILE_TYPE_SNE,
    STE_SIMPB_FILE_TYPE_PBC,
    STE_SIMPB_FILE_TYPE_FDN,
    STE_SIMPB_FILE_TYPE_EXT2,
    STE_SIMPB_FILE_TYPE_CCP2,
    STE_SIMPB_FILE_TYPE_INVALID,

    STE_SIMPB_FILE_TYPE_MAX
} ste_simpb_file_type_t;

typedef enum {
    STE_SIMPB_CAUSE_UNKNOWN,
    STE_SIMPB_CAUSE_REQUEST_RESPONSE,
    STE_SIMPB_CAUSE_UNSOLICITED_STATUS,
    STE_SIMPB_CAUSE_UNSOLICITED_SIM_STATE,
    STE_SIMPB_CAUSE_UNSOLICITED_SIMPB_FILE_IDS,
} ste_simpb_cause_t;

typedef enum {
    STE_SIMPB_REQUEST_ID_UNKNOWN = 0x00,
    STE_SIMPB_REQUEST_ID_READ,
    STE_SIMPB_REQUEST_ID_UPDATE,
    STE_SIMPB_REQUEST_ID_STATUS_GET,
    STE_SIMPB_REQUEST_ID_FILE_INFORMATION_GET,
    STE_SIMPB_REQUEST_ID_PHONEBOOK_SELECT,
    STE_SIMPB_REQUEST_ID_SELECTED_PHONEBOOK_GET,
    STE_SIMPB_REQUEST_ID_SUPPORTED_PHONEBOOKS_GET,

    STE_SIMPB_REQUEST_ID_STARTUP,
    STE_SIMPB_REQUEST_ID_SHUTDOWN,
    STE_SIMPB_REQUEST_ID_CACHE_PBR,

    STE_SIMPB_REQUEST_ID_SYNC_EF_PBC,
    STE_SIMPB_REQUEST_ID_CACHE_UPDATE_UID,
    STE_SIMPB_REQUEST_ID_END_OF_LIST
} simpbd_request_id_t;

/* Private data type which can contain data such as socket numbers.
   Used with all API calls to simpb. */
typedef struct ste_simpb_s ste_simpb_t;

typedef uint32_t simpb_client_tag_t;

typedef struct {  /* used as in parameter for the request functions */
    ste_simpb_phonebook_type_t  phonebook_type;
    ste_simpb_file_type_t       file_type;
    uint8_t                     length;
    uint8_t                     slice;  /* record of PBR */
    uint8_t                     instance; /* For EFs with multiple entries */
    uint16_t                    record_number;
    uint8_t                     p2;
} ste_simpb_file_info_t;

/* Callback data types for asynchronous mode*/
typedef struct {
    uint8_t                sw1;
    uint8_t                sw2;
    ste_simpb_iso_t        sim_iso_error;
    uint8_t                data_size;
    uint8_t                data_p[1];
} ste_simpb_cb_read_data_t;

typedef struct {
    uint8_t                sw1;
    uint8_t                sw2;
    ste_simpb_iso_t        sim_iso_error;
} ste_simpb_cb_update_data_t;

typedef struct {
    uint8_t                sw1;
    uint8_t                sw2;
    ste_simpb_iso_t        sim_iso_error;
    uint8_t                data_size;
    uint8_t                data_p[1];
} ste_simpb_cb_file_information_data_t;

typedef struct {
    uint8_t                    sw1;
    uint8_t                    sw2;
    ste_simpb_iso_t            sim_iso_error;
    ste_simpb_phonebook_type_t selected_phonebook;
    uint16_t                   number_of_entries;        /* Not used */
    uint16_t                   max_number_of_entries;    /* Not used */
} ste_simpb_cb_selected_phonebook_data_t;

typedef struct {
    ste_simpb_iso_t            sim_iso_error;
} ste_simpb_cb_phonebook_select_data_t;

typedef struct {
    ste_simpb_iso_t        sim_iso_error;
    uint8_t                supported_phonebooks;  /* binary map */
} ste_simpb_cb_supported_phonebooks_data_t;

typedef struct {
    ste_simpb_status_t     status;  /* the status of the relation with simpbd */
    ste_simpb_sim_state_t  sim_state; /* the state of simd */
} ste_simpb_cb_status_data_t;

typedef struct {
    ste_simpb_sim_state_t     value;
} ste_simpb_cb_sim_state_t;


typedef struct {
    uint8_t no_of_pb_files;
    uint16_t pb_file_ids[PB_MAX_FILE_IDS];
} simpb_file_ids_t;

typedef struct {
    uint16_t file_id;
    uint8_t record_num;
} simpb_adn_update_data_t;


/* Call-back type, "closure function" */
typedef void (*ste_simpb_cb_t)(ste_simpb_cause_t     cause,
                               uintptr_t             client_tag, /* client_tag == 0 is UNSOLICITED EVENT */
                               simpbd_request_id_t   request_id,
                               void                 *data_p,
                               ste_simpb_result_t    result,
                               void                 *client_context);

ste_simpb_result_t ste_simpb_connect(ste_simpb_t        **simpb_pp,
                                     int                 *simpb_fd,
                                     ste_simpb_status_t  *status_p,
                                     ste_simpb_cb_t       client_cb,
                                     void                *client_context);

ste_simpb_result_t ste_simpb_disconnect(ste_simpb_t *simpb_p);

ste_simpb_result_t ste_simbp_receive(ste_simpb_t *ste_simpb_p); /* Handler to be called by the client on selector to read data from service */


/* function to make simpbd to connect to simd.
 * Response type is ste_simpb_cb_sim_state_t.
 */
ste_simpb_result_t ste_simpb_startup(ste_simpb_t        *simpb_p,
                                     uintptr_t           client_tag);

/* function to make simpbd to disconnect from simd.
 * Response type is ste_simpb_cb_status_data_t.
 */
ste_simpb_result_t ste_simpb_shutdown(ste_simpb_t *simpb_p,
                                      uintptr_t    client_tag);

/* function to cache the PBR.
 */
ste_simpb_result_t ste_simpb_cache_pbr(ste_simpb_t *simpb_p, uintptr_t client_tag);

/* function to update UID file in ADN modified.
 */
ste_simpb_result_t ste_simpb_update_uids(ste_simpb_t *simpb_p, uintptr_t client_tag, uint16_t file_id, uint8_t rec_num, char *file_path);

/* Function to get state of simpbd communication.
 * Returns ste_simpb_status_t, STE_SIMPB_STATUS_INVALID if NULL pointer to ste_simpb_t is passed.
 */
ste_simpb_status_t ste_simpb_state_get(ste_simpb_t *simpb_p);

/* Function to get file descriptor used for simpbd communication.
 * Returns file descriptor or -1 if no connection.
 */
int ste_simpb_fd_get(ste_simpb_t *simpb_p);

/* Function to get simpb's status.
 * Response type is ste_simpb_cb_status_data_t.
 */
ste_simpb_result_t ste_simpb_status_get(ste_simpb_t *simpb_p,
                                        uintptr_t    client_tag);

/* Function to select the phone book.
 * If a 2G GSM card is being used, a simulated Global phone book will be present, but no Local phone book.
 * Response type is ste_simpb_cb_phonebook_select_data_t.
 */
ste_simpb_result_t ste_simpb_phonebook_select(ste_simpb_t                *simpb,
        uintptr_t                   client_tag,
        ste_simpb_phonebook_type_t  phonebook_type);

/* Function to get currently selected phone book.
 * Response type is ste_simpb_cb_selected_phonebook_data_t.
 */
ste_simpb_result_t ste_simpb_phonebook_selected_get(ste_simpb_t                *simpb_p,
        uintptr_t                   client_tag);

/* Function to get information about phone books supported by UICC.
 * If a 2G GSM card is being used, a simulated Global phone book will be present, but no Local phone book.
 * Response type is ste_simpb_cb_supported_phonebooks_data_t.
 */
ste_simpb_result_t ste_simpb_supported_phonebooks_get(ste_simpb_t *simpb,
        uintptr_t    client_tag);

/* Function to get information about the EF files (number of records and record length).
 * Response type is ste_simpb_cb_file_information_data_t.
 */
ste_simpb_result_t ste_simpb_file_information_get(ste_simpb_t            *simpb_p,
        uintptr_t               client_tag,
        ste_simpb_file_info_t  *file_info_p);


/* Function to read a record from a file.
 * Response type is ste_simpb_cb_read_data_t.
 */
ste_simpb_result_t ste_simpb_read(ste_simpb_t           *simpb,
                                  uintptr_t              client_tag,
                                  ste_simpb_file_info_t *file_info);

/* Function to write record to a file.
 * Response type is ste_simpb_cb_write_data_t.
 */
ste_simpb_result_t ste_simpb_update(ste_simpb_t           *simpb_p,
                                    uintptr_t              client_tag,
                                    ste_simpb_file_info_t *file_info_p,
                                    uint8_t               *data_p,
                                    uint8_t                data_size);

#endif /* _INCLUSION_GUARD_SIMPB_H_ */
