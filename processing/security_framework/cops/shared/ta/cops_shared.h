/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/
#ifndef COPS_SHARED_H
#define COPS_SHARED_H
/* Due to OPA file name collision we need this */
#ifdef COPS_USE_AUTO_GEN_INCLUDE
#include <cops_types_auto_gen.h>
#else
#include <cops_types.h>
#endif
#include <cops_data.h>
#include <cops_sipc_message.h>
#ifndef COPS_TAPP_EMUL
#ifndef COPS_ENABLE_FEATURE_STUBBED_SECURE_WORLD
#include <tee_client_api.h>
#endif
#endif

/*
 * The following macro is used to remove const cast-away warnings
 * from gcc -Wcast-qual; it should be used with caution because it
 * can hide valid errors; in particular most valid uses are in
 * situations where the API requires it, not to cast away string
 * constants.
 */
#define COPS_UNCONST(a) ((void *)(intptr_t)(const void *)(a))

/* This macro will issue a compiler error if x is false */
#define COPS_COMPILE_TIME_ASSERT(x) \
            {switch (0) {case 0:; case ((x) ? 1:0) : ; default:; } }

#define COPS_TAPP_IO_ARGS_SIZE  (1024 * 2)
#define COPS_TAPP_IO_DATA_SIZE  (1024 * 4)
/* if debug printouts from secure world are enabled, increase rvs
   size if needed */
#define COPS_TAPP_IO_RVS_SIZE   1024
#define COPS_TAPP_IO_PERM_AUTH_STATE_DATA_SIZE 1024
#ifdef COPS_TAPP_EMUL
#define COPS_TAPP_IO_STATE_SIZE 640
#endif

/* 48 */
#define COPS_CHALLENGE_SIZE (4 + (96/8) + 32)

typedef struct cops_tapp_io {
#ifdef COPS_TAPP_EMUL
    /* State of COPS tapp:s */
    uint8_t   state[COPS_TAPP_IO_STATE_SIZE];
#endif
    /* Arguments to tapp */
    cops_data_t args;
    /* COPS secured data */
    cops_data_t data;
    /* Return values from tapp */
    cops_data_t rvs;
    /* Auth data */
    cops_data_t perm_auth_state_data;
} cops_tapp_io_t;

typedef enum cops_argument_parameter_id {
    COPS_ARGUMENT_ID_ERROR_STACK = 0xF000,  /* COPS error code, file and line */
    COPS_ARGUMENT_ID_ERROR_STRING,          /* COPS error string */
    COPS_ARGUMENT_ID_ISSW_ERROR_STACK,      /* ISSW error code (TEEC_Result) */
    COPS_ARGUMENT_ID_SLA_ERROR_STACK,       /* ROM error code (SLA_RET) */

    COPS_ARGUMENT_ID_SIPC_META_DATA = 0xF010,
    COPS_ARGUMENT_ID_SIPC,                  /* Argument data to/from TAF's */
} cops_argument_id_t;

typedef enum cops_taf_id {
    COPS_SERVICE_READ_IMEI = 0,
    COPS_SERVICE_BIND_PROPERTIES = 1,
    COPS_SERVICE_VERIFY_IMSI = 3,
    COPS_SERVICE_READ_MODEM_DATA = 4,
    COPS_SERVICE_GET_SEED = 5,
    COPS_SERVICE_BIND_DATA = 6,
    COPS_SERVICE_VERIFY_DATA_BINDING = 7,
    COPS_SERVICE_AUTHENTICATE = 8,
    COPS_SERVICE_DEAUTHENTICATE = 9,
    COPS_SERVICE_CALCULATE_HASH = 11,
    COPS_SERVICE_GET_CHALLENGE = 12,
    COPS_SERVICE_SIMLOCK_UNLOCK = 13,
    COPS_SERVICE_SIMLOCK_GET_STATUS = 14,
    COPS_SERVICE_SIMLOCK_VERIFY_CONTROL_KEYS = 15,
    COPS_SERVICE_GET_DEVICE_STATE = 16,
#ifdef EXTENDED_SIPC_MSGS
    COPS_SERVICE_SET_MODEM_BAND = 17,
    COPS_SERVICE_SIMLOCK_GET_LOCK_SETTINGS = 18,
    COPS_SERVICE_SIMLOCK_GET_LOCK_ATTEMPTS_LEFT = 19,
#endif
    COPS_SERVICE_SIMLOCK_LOCK = 20,
#ifdef EXTENDED_SIPC_MSGS
    COPS_SERVICE_SIMLOCK_MCK_RESET = 21,
#endif
    COPS_SERVICE_VERIFY_SIGNED_HEADER = 22,
    COPS_SERVICE_READ_DATA = 23,
    COPS_SERVICE_INIT_ARB_TABLE = 24,
    COPS_SERVICE_CHECK_ARB_UPDATE = 25,
    COPS_SERVICE_WRITE_SECPROFILE = 26,
    COPS_SERVICE_SIMLOCK_CHANGE_CONTROL_KEY = 27,
    COPS_SERVICE_COPS_OTP = 28,
    COPS_SERVICE_HANDLE_SIMDATA = 29,
    COPS_SERVICE_ROUTER = 30,
    COPS_SERVICE_WRITE_RPMB_KEY = 31,
    COPS_SERVICE_GET_PRODUCT_DEBUG_SETTINGS = 32,
    COPS_SERVICE_DERIVE_SESSION_KEY = 33,
    COPS_SERVICE_GET_LONG_TERM_SHARED_KEY = 34,
    COPS_SERVICE_GET_SESSION_KEY_MODEM_RAND = 35,
    COPS_SERVICE_NUM_SERVICES
} cops_taf_id_t;

typedef struct cops_taf_read_imei_result {
    cops_return_code_t return_code;
    cops_data_imei_t imei;
} cops_taf_read_imei_result_t;

/* Mark that when changing this type the correponding type
   cops_simlock_card_status_t in cops_types.h also need to be
   changed */
typedef uint32_t cops_taf_simlock_card_status_t;

/* Mark that when changing this type the correponding type
   cops_simlock_lock_status_t in cops_types.h also need to be
   changed */
typedef struct cops_taf_simlock_lock_status {
    uint8_t lock_type;
    cops_simlock_lock_definition_t lock_definition;
    uint8_t lock_setting;
    size_t    attempts_left;
    bool      timer_is_running;
    uint32_t    time_left_on_running_timer;
    size_t    timer_attempts_left;
} cops_taf_simlock_lock_status_t;

/* Mark that when changing this type the correponding type
   cops_simlock_status_t in cops_types.h also need to be
   changed */
typedef struct cops_taf_simlock_status {
    cops_taf_simlock_card_status_t sim_card_status;
    cops_taf_simlock_lock_status_t nl_status;
    cops_taf_simlock_lock_status_t nsl_status;
    cops_taf_simlock_lock_status_t spl_status;
    cops_taf_simlock_lock_status_t cl_status;
    cops_taf_simlock_lock_status_t siml_status;
    cops_taf_simlock_lock_status_t esll_status;
} cops_taf_simlock_status_t;


typedef struct cops_arb_table_info {
    char *tocids[2];
    size_t tocidsize[2];
    uint8_t *tables[2];
    size_t tablesize[2];
} cops_arb_table_info_t;

typedef struct simlock_settings {
    /* -1 since testsim is not included */
    uint8_t lock_mode[COPS_NUMBER_OF_SIMLOCKS-1];
} simlock_settings_t;

typedef struct simlock_attempts {
    /* -1 since testsim is not included */
    uint32_t simlock_attempts[COPS_NUMBER_OF_SIMLOCKS-1];
} simlock_attempts_t;

cops_return_code_t cops_mac_calc(cops_mac_type_t mac_type,
                                 const uint8_t *data, size_t data_len,
                                 uint8_t *mac, size_t mac_len);

cops_return_code_t cops_mac_verify(cops_mac_type_t mac_type,
                                   const uint8_t *data, size_t data_len,
                                   const uint8_t *mac, size_t mac_len,
                                   bool *mac_ok);

cops_return_code_t cops_tapp_sipc_get_value_pointer(cops_sipc_message_t *msg,
        size_t *offset,
        uint8_t **value,
        size_t vlength);

cops_return_code_t cops_tapp_sipc_set_opaque(cops_sipc_message_t *msg,
        size_t *offset,
        const void *value, size_t vlength);

cops_return_code_t cops_tapp_sipc_get_opaque(const cops_sipc_message_t *msg,
        size_t *offset,
        void *value, size_t vlength);

cops_return_code_t cops_tapp_sipc_set_bool(cops_sipc_message_t *msg,
        size_t *offset, bool value);

cops_return_code_t cops_tapp_sipc_get_bool(const cops_sipc_message_t *msg,
        size_t *offset, bool *value);

cops_return_code_t cops_tapp_sipc_set_uint32(cops_sipc_message_t *msg,
        size_t *offset, uint32_t value);

cops_return_code_t cops_tapp_sipc_get_uint32(const cops_sipc_message_t *msg,
        size_t *offset, uint32_t *value);

cops_return_code_t cops_tapp_sipc_set_uint16(cops_sipc_message_t *msg,
        size_t *offset, uint16_t value);
cops_return_code_t cops_tapp_sipc_get_uint16(const cops_sipc_message_t *msg,
        size_t *offset, uint16_t *value);

cops_return_code_t cops_tapp_sipc_set_uint8(cops_sipc_message_t *msg,
        size_t *offset, uint8_t value);

cops_return_code_t cops_tapp_sipc_get_uint8(const cops_sipc_message_t *msg,
        size_t *offset, uint8_t *value);

#ifndef COPS_TAPP_EMUL
#ifndef COPS_ENABLE_FEATURE_STUBBED_SECURE_WORLD

void convertTeeOp2CopsIO_n2s(TEEC_Operation *param, cops_tapp_io_t *cops_io);

void convertCopsIO2TeeOp_n2s(cops_tapp_io_t **cops_io, TEEC_Operation *param);

bool convertTeeOp2CopsIO_s2n(TEEC_Operation *param, cops_tapp_io_t *cops_io);

void convertCopsIO2TeeOp_s2n(cops_tapp_io_t *cops_io, TEEC_Operation *param);

#endif
#endif

/*
 * List of files in cops_ta and tapp_test. This mapping is used because
 * cops_ta/tapp_test wont send filename back to normal world when
 * COPS_DEBUG isn't set. Hence we need to know which file caused the error.
 * If new files are created, ensure that the file is added to the list and
 * the appropriate #ifndef stuff added to the new file too.
 * Also update cops_debug.c
 */

typedef enum {
    /* from cops_shared */
    cops_data_imei_c = 0,
    cops_data_lock_c,
    cops_data_parameter_c,
    cops_data_sipc_c,
    cops_sipc_message_c,
    cops_tapp_sipc_c,

    /* from cops_ta */
    cops_arbhandling_c,
    cops_auth_c,
    cops_error_c,
    cops_extint_emul_c,
    cops_extint_secure_c,
    cops_otp_c,
    cops_ram_otp_c,
    cops_otp_stubbed_c,
    cops_protect_c,
    cops_simdata_util_c,
    cops_simlock_c,
    cops_state_emul_c,
    cops_state_secure_c,
    cops_trusted_application_c,
    otp_permissions_c,
    taf_authenticate_c,
    taf_bind_data_c,
    taf_bind_properties_c,
    taf_calchash_c,
    taf_cops_otp_c,
    taf_cops_otp_stubbed_c,
    taf_check_arb_update_c,
    taf_check_arb_update_stubbed_c,
    taf_deauthenticate_c,
    taf_derive_session_key_c,
    taf_derive_session_key_stubbed_c,
    taf_get_challenge_c,
    taf_get_device_state_c,
    taf_get_long_term_shared_key_c,
    taf_get_long_term_shared_key_stubbed_c,
    taf_get_seed_c,
    taf_get_session_key_modem_rand_c,
    taf_get_session_key_modem_rand_stubbed_c,
    taf_handle_simdata_c,
    taf_init_arb_table_c,
    taf_init_arb_table_stubbed_c,
    taf_read_data_c,
    taf_read_imei_c,
    taf_read_modem_data_c,
    taf_router_c,
    taf_set_modem_band_c,
    taf_simlock_change_sim_control_key_c,
    taf_simlock_get_lock_attempts_left_c,
    taf_simlock_get_lock_settings_c,
    taf_simlock_get_status_c,
    taf_simlock_lock_c,
    taf_simlock_mck_reset_c,
    taf_simlock_unlock_c,
    taf_simlock_verify_control_keys_c,
    taf_verify_data_binding_c,
    taf_verify_imsi_c,
    taf_verify_signedheader_c,
    taf_write_secprofile_c,
    taf_write_secprofile_stubbed_c,
    taf_write_rpmb_key_c,
    taf_write_rpmb_key_stubbed_c,
    taf_get_product_debug_settings_c,
    tapp_test_c
} cops_files_t;

enum cops_otp_command {
    READ_OTP = 0,
    WRITE_OTP,
    GET_NBR_OF_OTP_ROWS
};
#endif                          /*COPS_SHARED_H */
