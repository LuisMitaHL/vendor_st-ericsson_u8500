/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/
#ifndef COPS_DATA_H
#define COPS_DATA_H

#ifdef COPS_TAPP_EMUL
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#endif
#include <cops_sipc_message.h>

#define COPS_STORAGE_IMSI_LENGTH (15)

#define COPS_DATA_MAX_LENGTH    (5 * 1024)

typedef struct cops_data {
    uint8_t  *data;
    size_t    length;
    size_t    max_length;
} cops_data_t;

/**
 * @brief Identifies an individual parameter of COPS Secured Data.
 *
 * See "Platform security" chapter "COPS data file" for different
 * values of ParameterID.
 */
typedef uint32_t cops_parameter_id_t;

/* Keep in sync with COPS_Storage_ParameterID_t */
typedef enum cops_storage_parameter_id {
    COPS_PARAMETER_ID_UNDEFINED = 0x00,

    /* cops_data_imei_t */
    COPS_PARAMETER_ID_IMEI = 0x01,
    /* Hole */

    /* char key[8] */
    COPS_PARAMETER_ID_SIMLOCK_DEFAULT_KEY = 0x04,

    /* Complex structure used internally in cops_simlock.c */
    COPS_PARAMETER_ID_LOCK_SETTING_NL = 0x10,
    COPS_PARAMETER_ID_LOCK_SETTING_NSL,
    COPS_PARAMETER_ID_LOCK_SETTING_SPL,
    COPS_PARAMETER_ID_LOCK_SETTING_CL,
    COPS_PARAMETER_ID_LOCK_SETTING_SIML,
    COPS_PARAMETER_ID_LOCK_SETTING_ESLL,
    COPS_PARAMETER_ID_LOCK_SETTING_TESTSIM,

    /* cops_data_lock_control_t */
    COPS_PARAMETER_ID_LOCK_CONTROL_NL = 0x20,
    COPS_PARAMETER_ID_LOCK_CONTROL_NSL,
    COPS_PARAMETER_ID_LOCK_CONTROL_SPL,
    COPS_PARAMETER_ID_LOCK_CONTROL_CL,
    COPS_PARAMETER_ID_LOCK_CONTROL_SIML,
    COPS_PARAMETER_ID_LOCK_CONTROL_ESLL,

    /* uint32_t which is treaded as a bitfield see below */
    COPS_PARAMETER_ID_LOCK_DEFINITION_NL = 0x30,
    COPS_PARAMETER_ID_LOCK_DEFINITION_NSL,
    COPS_PARAMETER_ID_LOCK_DEFINITION_SPL,
    COPS_PARAMETER_ID_LOCK_DEFINITION_CL,
    COPS_PARAMETER_ID_LOCK_DEFINITION_SIML,

    COPS_STORAGE_PARAMETER_ID_SUBDOMAIN = 0x40,
    COPS_STORAGE_PARAMETER_ID_TPID = 0x41,

    /* mck_attempts_t */
    COPS_STORAGE_PARAMETER_ID_MCK_ATTEMPTS = 0x50,

    COPS_PARAMETER_ID_MODEM_DATA = 0x1000,

    COPS_PARAMETER_ID_USER_DEFINED_0 = 0x9FF0,
    COPS_PARAMETER_ID_USER_DEFINED_1,
    COPS_PARAMETER_ID_USER_DEFINED_2,
    COPS_PARAMETER_ID_USER_DEFINED_3,
    COPS_PARAMETER_ID_USER_DEFINED_4,
    COPS_PARAMETER_ID_USER_DEFINED_5,
    COPS_PARAMETER_ID_USER_DEFINED_6,
    COPS_PARAMETER_ID_USER_DEFINED_7,
    COPS_PARAMETER_ID_USER_DEFINED_8,
    COPS_PARAMETER_ID_USER_DEFINED_9,


    /* COPS_CORE ensures that SWBP can only access data up to this enum */
    COPS_PARAMETER_ID_USER_ACCESS_LEVEL = 0xA000,
} cops_storage_parameter_id_t;

typedef enum cops_protect_parameter_id {
    /* uint8_t mac[COPS_PROTECT_MAC_SIZE] */
    COPS_PARAMETER_ID_SIMLOCK_KEY_NL = 0xA100,
    COPS_PARAMETER_ID_SIMLOCK_KEY_NSL = 0xA102,
    COPS_PARAMETER_ID_SIMLOCK_KEY_SPL = 0xA104,
    COPS_PARAMETER_ID_SIMLOCK_KEY_CL = 0xA106,
    COPS_PARAMETER_ID_SIMLOCK_KEY_SIML = 0xA108,
    COPS_PARAMETER_ID_SIMLOCK_KEY_ESLL = 0xA10A,

    /* uint8_t mac[COPS_PROTECT_MAC_SIZE] */
    COPS_PARAMETER_ID_SIMLOCK_MASTER_CONTROL_KEY = 0xA200,

    /*
     * Used to store the permanent authentication status when OTP
     * can't hold it.
     */
    COPS_PARAMETER_ID_PERMANENT_AUTHENTICATION = 0xA500,

    /*
     * Counter of data to detect that data wasn't written to
     * flash as expected.
     */
    COPS_PARAMETER_ID_COUNTER,

    /*
     * This parameter should be the last in COPS data so it easily can
     * be excluded when calculating/checking COPS data MAC.
     */
    /* uint8_t mac[COPS_PROTECT_MAC_SIZE] */
    COPS_PARAMETER_ID_MAC = 0xFFFF
} cops_protect_parameter_id_t;

#define COPS_PARAMETER_HEADER_SIZE  4

/** @brief Container for an individual parameter of COPS Secure Data. */
typedef struct cops_parameter_data {
    /** @brief Parameter ID */
    cops_parameter_id_t id;

    /** @brief Length parameter data */
    size_t    length;

    /** @brief Pointer parameter data */
    uint8_t  *data;
} cops_parameter_data_t;

/**
 * @brief Initializes an empty COPS Data
 *
 * This function initializes and empty COPS Data. The function
 * requires COPS Data to be empty (cd->length = 0) or the function
 * will fail.
 *
 * @param [in,out]  cd  COPS Data
 *
 * @return cops_return_code_t
 */
cops_return_code_t cops_data_init(cops_data_t *cd);

/**
 * @brief Locates specified parameter in COPS Data
 *
 * This function locates a parameter in COPS Data. The parameter ID to look
 * for is specified in data->parameter_id.
 *
 * If the function returns TRUE:
 * \li data->data points to the payload of the parameter inside cd->data or
 *     NULL if parameter was not found.
 * \li data->length set to the length of payload of the paramter if
 *     cd->data != NULL
 *
 * Note that if COPS Data is not in secure memory the contents need to
 * be copied and verfied before it can be used.
 *
 * @param [in]     cd   COPS Data
 * @param [in,out] data Parameter data.
 *
 * @return cops_return_code_t
 */
cops_return_code_t cops_data_get_parameter(const cops_data_t *cd,
        cops_parameter_data_t *data);

/**
 * @brief Returns next parameter in COPS Data
 *
 * The function returns next parameter in COPS Data. If *offset is
 * 0 the first parameter is returned. If data->data is returned
 * as NULL end of COPS Data has been reached.
 *
 * @param [in]      cd          COPS Data
 * @param [in,out]  Offset      Offset in COPS Data
 * @param [out]     Parameter   Data
 *
 * @return cops_return_code_t
 */
cops_return_code_t cops_data_get_next_parameter(const cops_data_t *cd,
        size_t *offset,
        cops_parameter_data_t *data);

/**
 * @brief Updates or add a parameter to COPS Data
 *
 * This function updates or adds a paramter to COPS data. Parameter ID
 * and payload data is specified in data.
 *
 * Note that if the function returns FALSE COPS data may be
 * modified but not completely updated.
 *
 * @param [in,out] cd   COPS Data
 * @param [in,out] data Parameter data.
 *
 * @return cops_return_code_t
 */
cops_return_code_t cops_data_set_parameter(cops_data_t *cd,
        cops_parameter_data_t *data);

/**
 * @brief Add a parameter to COPS Data
 *
 * This function adds a parameter to COPS data.
 *
 * Note that this function allows duplicated parameter ids.
 *
 * @param [in,out] cd   COPS Data
 * @param [in,out] data Parameter data.
 *
 * @return cops_return_code_t
 */
cops_return_code_t cops_data_add_parameter(cops_data_t *cd,
        cops_parameter_data_t *data);

/**
 * @brief Remove a parameter from COPS Data
 *
 * This function removes a parameter from COPS data.
 *
 * @param [in,out] cd   COPS Data
 * @param [in,out] data Parameter data.
 *
 * @return cops_return_code_t
 */
cops_return_code_t cops_data_remove_parameter(cops_data_t *cd,
                                              cops_parameter_data_t *d);


/* Keep in sync with COPS_Storage_IMEI_t */
typedef struct cops_data_imei {
    uint8_t   imei[6];
} cops_data_imei_t;

cops_return_code_t cops_data_get_imei(const cops_data_t *cd,
                                      cops_data_imei_t *imei);
cops_return_code_t cops_data_set_imei(cops_data_t *cd, cops_data_imei_t *imei);

cops_return_code_t
cops_data_lock_type_to_setting_paramid(cops_simlock_lock_type_t lt,
                                       cops_parameter_id_t *id);

cops_return_code_t
cops_data_lock_type_to_control_paramid(cops_simlock_lock_type_t lt,
                                       cops_parameter_id_t *id);

cops_return_code_t
cops_data_lock_type_to_definition_paramid(cops_simlock_lock_type_t lt,
        cops_parameter_id_t *id);

cops_return_code_t cops_data_lock_type_to_key_paramid(cops_simlock_lock_type_t
        lt,
        cops_parameter_id_t *id);

/**
 * @brief Mode of a SIMLock lock.
 *
 * Used for the settings of the SIMLock locks.
 */
typedef enum cops_data_lock_mode {
    /** @brief The lock is unlocked (inactive). */
    COPS_DATA_LOCK_MODE_UNLOCKED,

    /**
     * @brief The lock is locked.
     */
    COPS_DATA_LOCK_MODE_LOCKED,

    /** @brief The lock is permanently disabled. */
    COPS_DATA_LOCK_MODE_DISABLED,

    /**
     * @brief The lock will be locked to the inserted SIM at next startup.
     *
     * Not applicable for SIM Personalization lock
     */
    COPS_DATA_LOCK_MODE_AUTOLOCK_ENABLED,

    /**
     * @brief The lock will be locked to the CNL of inserted SIM
     *        at next startup.
     *
     * Not applicable for SIM Personalization lock
     */
    COPS_DATA_LOCK_MODE_AUTOLOCK_TO_CNL_ENABLED,

    /* Reserved */
    COPS_DATA_LOCK_MODE_RESERVED1,

    COPS_DATA_LOCK_MODE_TESTSIM_BYPASS_SIMLOCK,
    COPS_DATA_LOCK_MODE_RESERVED2,
    COPS_DATA_LOCK_MODE_RESERVED3
} cops_data_lock_mode_t;

cops_return_code_t cops_data_get_lock_setting(const cops_data_t *cd,
        cops_simlock_lock_type_t lt,
        uint8_t *lock_setting);

/**
 * @brief Data stored in COPS_PARAMTER_ID_LOCK_CONTROL_XXX
 *
 * The timer value is increased with Addition and multiplied
 * with Multiplier/10 for each new try after the first one.
 */
typedef struct cops_data_lock_control {
    /** @brief Current number of performed attempts (default 0) */
    uint16_t  performed_attempts;

    /** @brief Initial value for the timer. */
    uint16_t  timer_start_value;

    /** @brief A timer is running for this lock */
    uint8_t   timer_running;

    /** @Number of allowed attempts for unlocking */
    uint8_t   allowed_attempts;

    /**
     * @brief Number of allowed attempts for locking/unlocking
     *        with timer between each attempt.
     *
     * Total number of attempts is
     * AllowedAttemts+AllowedTimerAttemts if
     * EnableRestartTimerAttempts is FALSE and
     * AllowedAttemts*AllowedTimerAttempts + AllowedTimerAttemts
     * if EnableRestartTimerAttempts is TRUE
     */
    uint8_t   allowed_timer_attempts;

    /**
     * @brief If set to true, when the timer expires, it will not
     * grant only one extra attempt, instead it will grant
     * allowed attempts yet again. If set to false only one extra
     * attempt is granted.
     */
    uint8_t   enable_restart_timer_attempts;

    /**
     * @brief Value that is added to the current timer period on
     * each retry.
     */
    uint8_t   addition;

    /**
     * @brief Value that is devided by 10 and multiplied with
     * current timer period on each retry.
     */
    uint8_t   multiplier;

    /**
     * @brief If set to true a timer should continue to run even
     * if the ME is shutdown.
     */
    uint8_t   enable_offline_timer;

    uint8_t   reserved[1];

    /**
     * @brief The value of the timer. This value is written when
     * the ME is shutting down.
     */
    uint32_t  current_timer_value_ms;

    /** @brief The time of ME (12:34:56) represented in seconds. */
    uint32_t  current_time;
} cops_data_lock_control_t;


cops_return_code_t cops_data_get_lock_control(const cops_data_t *cd,
        cops_simlock_lock_type_t lt,
        cops_data_lock_control_t *ctrl);

cops_return_code_t cops_data_set_lock_control(cops_data_t *cd,
        cops_simlock_lock_type_t lt,
        const cops_data_lock_control_t
        *ctrl);

/**
 * Lock defintion is  bitmask telling which positions in the IMSI
 * and if GID1 and GID2 should be used by the lock in question.
 *
 * Bit 0 to 14 indicates position 0 to 14 in the IMSI
 * Bit 15 Gid1
 * Bit 16 Gid2
 *
 *
 * This type describes the bitmaping of the lock definition data
 * which will be uesd to indentify which lock Regarding an
 * Normal-, Auto- Explicit lock will be performed
 *
 * Examples for Europe (2Digit MNC):
 *                         |MCC|MNC|NS|  MSIN  | GID1 | GID2
 *  Network Lock:           111 11  00 00000000   0      0
 *  Network Subset Lock:    111 11  11 00000000   0      0
 *  Service Provider Lock:  111 11  00 00000000   1      0
 *  Corporate Lock:         111 11  00 00000000   1      1
 *  (U)SIM Lock:            111 11  11 11111111   0      0
 *
 *  Network Lock:          0x 0000 001F
 *  Network Subset Lock:   0x 0000 007F
 *  Service Provider Lock: 0x 0000 801F
 *  Corporate Lock:        0x 0001 801F
 *  (U)SIM Lock:           0x 0000 7FFF
 *
 * Examples for US (3Digit MNC):
 *                         |MCC|MNC|NS|  MSIN  | GID1 | GID2
 *  Network Lock:           111 111 00 0000000    0      0
 *  Network Subset Lock:    111 111 11 0000000    0      0
 *  Service Provider Lock:  111 111 00 0000000    1      0
 *  Corporate Lock:         111 111 00 0000000    1      1
 *  (U)SIM Lock:            111 111 11 1111111    0      0
 *
 *  Network Lock:          0x 0000 003F
 *  Network Subset Lock:   0x 0000 00FF
 *  Service Provider Lock: 0x 0000 803F
 *  Corporate Lock:        0x 0001 803F
 *  (U)SIM Lock:           0x 0000 7FFF
 *
 */
#define COPS_DATA_LOCKDEF_IMSI_BIT(digit)   (1 << (digit))
#define COPS_DATA_LOCKDEF_GID1_BIT          (1 <<  15)
#define COPS_DATA_LOCKDEF_GID2_BIT          (1 <<  16)

cops_return_code_t cops_data_get_lock_definition(const cops_data_t *cd,
        cops_simlock_lock_type_t lt,
        uint32_t *lock_def);

cops_return_code_t cops_data_set_lock_definition(cops_data_t *cd,
        cops_simlock_lock_type_t lt,
        const uint32_t lock_def);

/**
 * @brief Data stored in COPS_STORAGE_PARAMETER_ID_MCK_ATTEMPTS
 *
 */
typedef struct mck_attempts {
    /** @brief Current number of performed attempts (default 0) */
    uint16_t  performed_attempts;

    /** @Number of allowed attempts for unlocking */
    uint8_t   allowed_attempts;

    uint8_t   reserved[1];

} mck_attempts_t;

cops_return_code_t
cops_data_get_mck_attempts(const cops_data_t *cd, mck_attempts_t *attempts);

cops_return_code_t
cops_data_set_mck_attempts(cops_data_t *cd, const mck_attempts_t *attempts);

cops_return_code_t cops_data_get_sipc_message(cops_data_t *cd,
                                              cops_sipc_message_t **msgp);

cops_return_code_t cops_data_set_sipc_message(cops_data_t *cd,
                                              cops_sipc_message_t *msgp);

cops_return_code_t cops_tapp_clear_args_rvs(cops_data_t *rvs);

void util_cops_pack_imei(const cops_imei_t *imei,
                         cops_data_imei_t *packed_imei);

void util_cops_unpack_imei(const cops_data_imei_t *packed_imei,
                           cops_imei_t *imei);


#endif                          /*COPS_DATA_H */
