#ifndef COPS_SIPC_MESSAGE_H
#define COPS_SIPC_MESSAGE_H
/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/
/* Due to OPA file name collision we need this */
#ifdef COPS_USE_AUTO_GEN_INCLUDE
#include <cops_types_auto_gen.h>
#else
#include <cops_types.h>
#endif

typedef enum cops_sipc_mtype {
    COPS_SIPC_READ_UNKNOWN = 0x00,

    /* Special messages that are handled a bit differentely */
    COPS_SIPC_TRIGGER_MSG = 0x01,
    COPS_SIPC_IPC_STATE_REQ = 0x02,
    COPS_SIPC_IPC_STATE_RESP,

    /* Generic requests from modem 0x10-0x2F */
    COPS_SIPC_READ_IMEI_MREQ = 0x10,
    COPS_SIPC_READ_IMEI_MRESP,
    COPS_SIPC_VERIFY_IMSI_MREQ,
    COPS_SIPC_VERIFY_IMSI_MRESP,
    COPS_SIPC_READ_MODEM_DATA_MREQ,
    COPS_SIPC_READ_MODEM_DATA_MRESP,
    COPS_SIPC_VERIFY_DATA_BINDING_MREQ,
    COPS_SIPC_VERIFY_DATA_BINDING_MRESP,
    COPS_SIPC_BIND_DATA_MREQ,
    COPS_SIPC_BIND_DATA_MRESP,
    COPS_SIPC_GET_SEED_MREQ,
    COPS_SIPC_GET_SEED_MRESP,
    COPS_SIPC_VERIFY_SIGNED_HEADER_MREQ,
    COPS_SIPC_VERIFY_SIGNED_HEADER_MRESP,
    COPS_SIPC_CALCULATE_HASH_MREQ,
    COPS_SIPC_CALCULATE_HASH_MRESP,

    /* Generic request from host 0x30-0x4F */
    COPS_SIPC_READ_SIM_DATA_HREQ = 0x30,
    COPS_SIPC_READ_SIM_DATA_HRESP,
    COPS_SIPC_INITIATE_IMSI_VERIFICATION_HREQ,
    COPS_SIPC_INITIATE_IMSI_VERIFICATION_HRESP,
    RESERVED1,
    RESERVED2,

    /* Customized requests from modem 0x50- */
    COPS_SIPC_SET_MODEM_BAND_MREQ = 0x50,
    COPS_SIPC_SET_MODEM_BAND_MRESP,
    COPS_SIPC_SIMLOCK_UNLOCK_MREQ,
    COPS_SIPC_SIMLOCK_UNLOCK_MRESP,
    COPS_SIPC_SIMLOCK_GET_LOCK_SETTINGS_MREQ,
    COPS_SIPC_SIMLOCK_GET_LOCK_SETTINGS_MRESP,
    COPS_SIPC_SIMLOCK_GET_LOCK_ATTEMPTS_LEFT_MREQ,
    COPS_SIPC_SIMLOCK_GET_LOCK_ATTEMPTS_LEFT_MRESP,
    COPS_SIPC_SIMLOCK_LOCK_MREQ,
    COPS_SIPC_SIMLOCK_LOCK_MRESP,
    COPS_SIPC_SIMLOCK_MCK_RESET_MREQ,
    COPS_SIPC_SIMLOCK_MCK_RESET_MRESP,
    COPS_SIPC_SIMLOCK_GET_STATUS_MREQ,
    COPS_SIPC_SIMLOCK_GET_STATUS_MRESP,
    COPS_SIPC_AUTHENTICATE_MREQ,
    COPS_SIPC_AUTHENTICATE_MRESP,

    COPS_SIPC_BIND_PROPERTIES_MREQ,
    COPS_SIPC_BIND_PROPERTIES_MRESP,
    COPS_SIPC_GET_NBR_OF_OTP_ROWS_MREQ,
    COPS_SIPC_GET_NBR_OF_OTP_ROWS_MRESP,
    COPS_SIPC_READ_OTP_MREQ,
    COPS_SIPC_READ_OTP_MRESP,
    COPS_SIPC_WRITE_OTP_MREQ,
    COPS_SIPC_WRITE_OTP_MRESP,
    COPS_SIPC_GET_CHALLANGE_MREQ,
    COPS_SIPC_GET_CHALLANGE_MRESP,
    COPS_SIPC_DEAUTHENTICATE_MREQ,
    COPS_SIPC_DEAUTHENTICATE_MRESP,
    COPS_SIPC_SIMLOCK_VERIFY_CONTROL_KEYS_MREQ,
    COPS_SIPC_SIMLOCK_VERIFY_CONTROL_KEYS_MRESP,
    COPS_SIPC_LOCK_BOOT_PART_MREQ,
    COPS_SIPC_LOCK_BOOT_PART_MRESP,

    COPS_SIPC_INIT_ARB_TABLE_MREQ,
    COPS_SIPC_INIT_ARB_TABLE_MRESP,
    COPS_SIPC_CHECK_ARB_UPDATE_MREQ,
    COPS_SIPC_CHECK_ARB_UPDATE_MRESP,
    COPS_SIPC_GET_DEVICE_STATE_MREQ,
    COPS_SIPC_GET_DEVICE_STATE_MRESP,
    COPS_SIPC_READ_DATA_MREQ,
    COPS_SIPC_READ_DATA_MRESP,
    COPS_SIPC_SIMLOCK_OTA_UNLOCK_MREQ,
    COPS_SIPC_SIMLOCK_OTA_UNLOCK_MRESP,
    COPS_SIPC_MODEM_SIPC_MX_MREQ,
    COPS_SIPC_MODEM_SIPC_MX_MRESP,
    COPS_SIPC_WRITE_SECPROFILE_MREQ,
    COPS_SIPC_WRITE_SECPROFILE_MRESP,
    COPS_SIPC_SIMLOCK_CHANGE_SIM_CTRL_KEY_MREQ,
    COPS_SIPC_SIMLOCK_CHANGE_SIM_CTRL_KEY_MRESP,
    COPS_SIPC_WRITE_RPMB_KEY_MREQ,
    COPS_SIPC_WRITE_RPMB_KEY_MRESP,

    COPS_SIPC_GET_PRODUCT_DEBUG_SETTINGS_MREQ,
    COPS_SIPC_GET_PRODUCT_DEBUG_SETTINGS_MRESP,

    COPS_SIPC_DERIVE_SESSION_KEY_MREQ,
    COPS_SIPC_DERIVE_SESSION_KEY_MRESP,
    COPS_SIPC_GET_LONG_TERM_SHARED_KEY_MREQ,
    COPS_SIPC_GET_LONG_TERM_SHARED_KEY_MRESP,
    COPS_SIPC_GET_SESSION_KEY_MODEM_RAND_MREQ,
    COPS_SIPC_GET_SESSION_KEY_MODEM_RAND_MRESP,

    COPS_SIPC_GENERIC_EVENT = 0xFF,
} cops_sipc_mtype_t;

typedef enum cops_sipc_protscm {
    COPS_SIPC_PROTSCM_NONE = 0,
    COPS_SIPC_PROTSCM_HMAC_SHA256
} cops_sipc_protscm_t;

typedef enum cops_sipc_hashscm {
    COPS_SIPC_HASHSCM_SHA1 = 0,
    COPS_SIPC_HASHSCM_SHA256
} cops_sipc_hashscm_t;

/*
 * @brief Secure or Unsecure sender
 * Message sent from modem or TA is secure
 * All other senders are unsecure
*/
enum cops_sender {
    COPS_SENDER_SECURE = 0,
    COPS_SENDER_UNSECURE
};

enum cops_trigger_msg {
    COPS_TRIGGER_VERIFY_IMSI_REQUEST = 0,
    COPS_TRIGGER_AUTHENTICATION_EVENT
};

typedef enum cops_mac_type {
    COPS_MAC_TYPE_SIPC = 0, /* used in MB/FB. And between ACC/DATA in 7400 */
    COPS_MAC_TYPE_SIPC_EXTERNAL, /* used between DATA/APE in 7400 (session
                                    key derived from Long term key) */
    COPS_MAC_TYPE_COPS_DATA,
    COPS_MAC_TYPE_BIND_DATA,
    COPS_MAC_TYPE_LONG_TERM_KEY
} cops_mac_type_t;

/** @brief Size of MAC in bytes */
#define COPS_SIPC_MAC_SIZE  (32)

/** @brief Returns pointer to payload data */
#define COPS_SIPC_DATA(msg) \
    ((uint8_t *)((cops_sipc_message_t *)(msg) + 1))
#define COPS_SIPC_CONST_DATA(msg) \
    ((const uint8_t *)((const cops_sipc_message_t *)(msg) + 1))

/** @brief Returns length of payload data */
#define COPS_SIPC_DATA_LENGTH(msg) \
    ((msg)->length - sizeof(cops_sipc_message_t) - COPS_SIPC_MAC_SIZE)

/**
 * @brief Returns pointer to MAC
 *
 * Returns NULL if prot_scm is not COPS_SIPC_PROTSCM_HMAC_SHA256
 */
#define COPS_SIPC_MAC(msg) \
    ((msg->prot_scm & 1) == COPS_SIPC_PROTSCM_HMAC_SHA256 ?                 \
        COPS_SIPC_DATA(msg) + COPS_SIPC_DATA_LENGTH(msg) : NULL)
#define COPS_SIPC_CONST_MAC(msg) \
    ((msg->prot_scm & 1) == COPS_SIPC_PROTSCM_HMAC_SHA256 ?             \
        COPS_SIPC_CONST_DATA(msg) + COPS_SIPC_DATA_LENGTH(msg) : NULL)

#define COPS_SIPC_IS_REQUEST(msg) \
    ((msg->msg_type % 2) == 0 ? true : false)

#define COPS_RUN_MODE_UNKNOWN 0
#define COPS_RUN_MODE_MODEM 1
#define COPS_RUN_MODE_RELAY 2
#define COPS_RUN_MODE_DAEMON 3

typedef struct cops_sipc_message {
    const uint8_t msg_type;
    const uint8_t prot_scm;
    const uint16_t length;
    uint32_t  counter;
/* Only used to visualize the layout of a coded message */
#if 0
    uint8_t   data[];           /* Variable length */

    /* if prot_scm == COPS_SIPC_PROTSCM_HMAC_SHA256 */
    uint8_t   mac[COPS_SIPC_MAC_SIZE]
#endif
} __attribute__((__packed__))
cops_sipc_message_t;

typedef struct cops_sipc_session {
    uint32_t  counter;
} cops_sipc_session_t;

cops_return_code_t cops_sipc_increment_session(cops_sipc_session_t *sess);

cops_return_code_t cops_sipc_alloc_message(cops_return_code_t rc,
                                           size_t data_length,
                                           cops_sipc_mtype_t msg_type,
                                           uint8_t sender,
                                           cops_sipc_message_t **msg);

void cops_sipc_free_message(cops_sipc_message_t **msg);

enum cops_sender cops_sipc_get_sender_from_msg(const cops_sipc_message_t *msg);

void cops_sipc_clear_sender_in_msg(cops_sipc_message_t *msg);

cops_return_code_t cops_sipc_protect_message(const cops_sipc_session_t *sess,
                                             cops_sipc_message_t *msg,
                                             bool mac_ok);

cops_return_code_t
cops_sipc_protect_message_with_key(const cops_mac_type_t mac_type,
                                   const cops_sipc_session_t *sess,
                                   cops_sipc_message_t *msg,
                                   bool mac_ok);

cops_return_code_t cops_sipc_verify_message(const cops_sipc_message_t *msg,
                                            cops_sipc_session_t *sess,
                                            bool *mac_ok,
                                            bool internal_counter);

cops_return_code_t
cops_sipc_verify_message_with_key(const cops_mac_type_t mac_type,
                                  const cops_sipc_message_t *msg,
                                  cops_sipc_session_t *sess,
                                  bool *mac_ok,
                                  bool internal_counter);

#endif                          /*COPS_SIPC_MESSAGE_H */
