/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/
#ifndef COPS_TYPES_H
#define COPS_TYPES_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define SIMLOCK_OTA_SIZE_OF_UNLOCK_KEY 8

/**
   @brief the size of maximum size of IMEI (unpacked)
*/

#define SIMLOCK_OTA_SIZE_OF_IMEI 16

/**
   @brief number of digits in an IMSI
*/
#define COPS_NUM_IMSI_DIGITS 15

/**
   @brief Length of unpacked IMEI
*/
#define COPS_UNPACKED_IMEI_LENGTH   (15)

/**
   @brief Length of packed IMSI
*/
#define COPS_PACKED_IMSI_LENGTH (9)

/**
 * @brief Maximum length (in number of characters) of a
 * SIMLock lock control key.
 */
#define COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH (16)

/**
 * @brief Minimum length (in number of characters) of a
 * SIMLock lock control key.
 */
#define COPS_SIMLOCK_CONTROLKEY_MIN_LENGTH (8)

/**
 * @brief Minimum length (in number of characters) of a
 * cops_simlock_control_key_t use by the SIM/USIM Personalization.
 */
#define COPS_SIMLOCK_SIM_CONTROLKEY_MIN_LENGTH (6)

/**
 * @brief the length of a hash in bytes (SHA-256)
 */
#define COPS_HASH_LENGTH_SHA256 (32)

/**
 * @brief the length of a hash in bytes (SHA-1)
 */
#define COPS_HASH_LENGTH_SHA1 (20)

/**
 * @brief the length of a digest in bytes
 */
#define COPS_DIGEST_LENGTH (32)

/**
 * @brief To be used as a help for header buffer (hdr)
 *        allocation in the verify signed header interface.
 *        This size applies to RSA-2048 bit signatures
 *        For RSA-1024 bit signatures the size is 208
 */
#define COPS_SIGNED_HEADER_MIN_BUF_SIZE (336)

/**
 * @brief Primitives sent from daemon process to
 *        API proxy process for synchronization
 */
#define COPS_IPC_READY_PRIMITIVE 0xFFFF
#define COPS_IPC_NOT_READY_PRIMITIVE 0xFFFE

/**
 * @brief COPS return codes.
 *
 * When a function return something different from COPS_RC_OK
 * it's always matched by an entry in the system log.
 */
typedef enum cops_return_code {
    /** @brief The operation succeeded. */
    COPS_RC_OK = 0,

    /** @brief Asyncronous call initiated. */
    COPS_RC_ASYNC_CALL, /* 1 */

    /** @brief IPC with COPS failed. */
    COPS_RC_IPC_ERROR, /* 2 */

    /** @brief Internal IPC or PSock error. */
    COPS_RC_INTERNAL_IPC_ERROR, /* 3 */

    /** @brief Asyncronous IPC is not supported/setup for this function */
    COPS_RC_ASYNC_IPC_ERROR, /* 4 */

    /** @brief Incorrect arguments for function. */
    COPS_RC_ARGUMENT_ERROR, /* 5 */

    /** @brief Storage error (read/write flash failed). */
    COPS_RC_STORAGE_ERROR, /* 6 */

    /** @brief Failed to allocate memory */
    COPS_RC_MEMORY_ALLOCATION_ERROR, /* 7 */

    /** @brief Unspecified error. */
    COPS_RC_UNSPECIFIC_ERROR, /* 8 */

    /** @brief Service not available (functionality stubbed or non-existent) */
    COPS_RC_SERVICE_NOT_AVAILABLE_ERROR, /* 9 */

    /** @brief Error in a service (COPS to COPS_TA interaction error) */
    COPS_RC_SERVICE_ERROR, /* 10 */

    /** @brief Not authenticated error. */
    COPS_RC_NOT_AUTHENTICATED_ERROR, /* 11 */

    /** @brief No or invalid challenge found (from a previous call to
        cops_get_challenge or inside a challenge response message) */
    COPS_RC_CHALLENGE_ERROR, /* 12 */
    COPS_RC_CHALLENGE_MISSING_ERROR = COPS_RC_CHALLENGE_ERROR,

    /** @brief Signature validation failed (e.g. a challenge is
        signed with a wrong certificate) */
    COPS_RC_SIGNATURE_VERIFICATION_ERROR, /* 13 */

    /** @brief Data tampered error. */
    COPS_RC_DATA_TAMPERED_ERROR, /* 14 */

    /** @brief Data configuration error. */
    COPS_RC_DATA_CONFIGURATION_ERROR, /* 15 */

    /** @brief SIM card is not OK with SIMLock settings. */
    COPS_RC_INCORRECT_SIM, /* 16 */

    /** @brief A simlock unlock timer is currently running. */
    COPS_RC_TIMER_RUNNING, /* 17 */

    /** @brief No simlock unlock attempts left. */
    COPS_RC_NO_UNLOCK_ATTEMPTS_LEFT, /* 18 */

    /** @brief The key is invalid. */
    COPS_RC_INVALID_SIMLOCK_KEY, /* 19 */

    /** @brief Internal error. */
    COPS_RC_INTERNAL_ERROR, /* 20 */

    /** @brief Not allowed to lock a simlock. */
    COPS_RC_LOCKING_ERROR, /* 21 */

    /** @brief  Still unlock attempts left for the lock */
    COPS_RC_UNLOCK_ATTEMPTS_STILL_LEFT, /* 22 */

    /** @brief Autolock only allowed during first boot */
    COPS_RC_AUTOLOCK_NOT_ALLOWED, /* 23 */

    /** @brief Unable to write to a row which is already locked */
    COPS_RC_OTP_LOCKED_ERROR, /* 24 */

    /** @brief Attempt to use memory buffer that are to small */
    COPS_RC_BUFFER_TOO_SMALL, /* 25 */

    /** @brief Expected data is missing (e.g. no IMEI data in COPS file) */
    COPS_RC_DATA_MISSING_ERROR, /* 26 */

    /** @brief Error in call to ISSW */
    COPS_RC_ISSW_ERROR, /* 27 */

    /** @brief Error in call to ROM code */
    COPS_RC_SLA_ERROR, /* 28 */

    /** @brief Error in call to TEE Client */
    COPS_RC_TEEC_ERROR, /* 29 */

    /** @brief Not enough permission when reading/writing OTP */
    COPS_RC_OTP_PERMISSION_ERROR, /* 30 */

    /** @brief Error when reading SIM data */
    COPS_RC_SIM_ERROR, /* 31 */


    COPS_RC_UNDEF = 255
} cops_return_code_t;

/**
 * @brief The operations to perform with SIMLock_Lock
 *
 * @brief COPS_SIMLOCK_LOCKOP_SETLOCKDATA Only store the new lock data and
 *        does not touch the lock setting. If CLEARLOCKDATA is set the previous
 *        data is removed before new data is added, otherwise it is appended
 * @brief COPS_SIMLOCK_LOCKOP_CLEARLOCKDATA Clear the previous lock data
 * @brief COPS_SIMLOCK_LOCKOP_SETLOCKSETTING Only change the lock setting
 *        to the provided value. Do not touch lock data
 * @brief Possible values:
 *        1 Store new lockdata. If data already exists, the new data will be
 *          appended to the old.
 *        2 Clear exisiting lockdata
 *        3 Replace the existing lockdata with new lockdata
 *        4 Change the locksetting, for example lock the lock
 *        5 Store new lockdata and change the locksetting, for example lock the
 *          lock. If data already exists, the new data will be appended to the
 *          old.
 *        6 Clear the lockdata and set the locksetting. Can be used when
 *          setting locksetting to autolock. Doesn't set any new lockdata.
 *        7 Replace the existing lockdata with new lockdata and change the
 *          locksetting, for example lock the lock.
 */
enum cops_simlock_lock_operation {
    COPS_SIMLOCK_LOCKOP_SETLOCKDATA = 1,
    COPS_SIMLOCK_LOCKOP_CLEARLOCKDATA = 2,
    COPS_SIMLOCK_LOCKOP_SETLOCKSETTING = 4,
};

/**
   @brief Contains the IMEI (International Mobile Equipment Identity).
*/
typedef struct cops_imei {
    /**
     * @brief The IMEI in unpacked format.
     *
     * Each byte represents a digit. Valid range of each byte is 0x0..0x9
     */
    uint8_t   digits[COPS_UNPACKED_IMEI_LENGTH];
} cops_imei_t;

/**
 * @brief A SIMLock lock control key
 *
 * The string must be '\0' terminated. The string length should
 * be in the range between SIMLOCK_CONTROLKEY_MIN_LENGTH and
 * SIMLOCK_CONTROLKEY_MAX_LENGTH.
 */
typedef struct cops_simlock_control_key {
    /**
     * @brief The control key
     *
     * Valid values are '0'..'9' and '\0'.
     */
    char      value[COPS_SIMLOCK_CONTROLKEY_MAX_LENGTH + 1];
} __attribute__((__packed__)) cops_simlock_control_key_t;

/**
 * @brief ControlKey to be used for reseting the SIM/USIM Personalization
 * ControlKey to the default value
 */
#define COPS_SIMLOCK_SIM_RESET_KEY \
    (&(cops_simlock_control_key_t) \
       { {0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x00, 0x00, \
          0x52, 0x45, 0x53, 0x45, 0x54, 0x00, 0x00, 0x00, 0x00} })

/**
 * @brief Type of a lock. Since data of cops_simlock_lock_type is casted to
 *        uint8_t, the values of this enum should never be greater than 255
 */
typedef enum cops_simlock_lock_type {
    /**
     * @brief Network lock
     *
     * Locks to the MCC and MNC part of the IMSI. The
     * interpretation of this lock type can be overridden with
     * lock_defintion in cops_simlock_lock_arg.
     */
    COPS_SIMLOCK_LOCK_TYPE_NETWORK_LOCK = 0,

    /**
     * @brief Network subset lock
     *
     * Locks to the MCC, MNC and NS part of the IMSI. The
     * interpretation of this lock type can be overridden with
     * lock_defintion in cops_simlock_lock_arg.
     */
    COPS_SIMLOCK_LOCK_TYPE_NETWORK_SUBSET_LOCK,

    /**
     * @brief Service provider lock
     *
     * Locks to the MCC and MNC part of the IMSI, and GID1. The
     * interpretation of this lock type can be overridden with
     * lock_defintion in cops_simlock_lock_arg.
     */
    COPS_SIMLOCK_LOCK_TYPE_SERVICE_PROVIDER_LOCK,

    /**
     * @brief Corporate lock
     *
     * Locks to the MCC and MNC part of the IMSI, and GID2. The
     * interpretation of this lock type can be overridden with
     * lock_defintion in cops_simlock_lock_arg.
     */
    COPS_SIMLOCK_LOCK_TYPE_CORPORATE_LOCK,

    /**
     * @brief SIM/USIM Personalization lock in 3GPP TS 22.022.
     *
     * Locks to the complete IMSI.
     */
    COPS_SIMLOCK_LOCK_TYPE_SIM_LOCK,

    /**
     * @brief SIM/USIM Personalization lock in 3GPP TS 22.022.
     *        Deprecated. Defined for backwards compability only,
     *        Subject for future removal - don't use!
     */
    COPS_SIMLOCK_LOCK_TYPE_USER_LOCK = COPS_SIMLOCK_LOCK_TYPE_SIM_LOCK,

    /**
     * @brief Flexible ESL lock
     *
     * Locks to a specified range of IMSI values.
     */
    COPS_SIMLOCK_LOCK_TYPE_FLEXIBLE_ESL_LOCK,

    /**
     * @brief Testsim, defines which sim card should be treated
     *         as test sim card.
     *
     * Matches against a specified range of IMSI values. This is not
     * a proper lock and not included in cops_simlock_status_t.
     */
    COPS_SIMLOCK_LOCK_TYPE_TESTSIM,
    /**
      * @brief Number of simlocks
      *
      * The number of different existing simlocks
      */
    COPS_NUMBER_OF_SIMLOCKS
} cops_simlock_lock_type_t;

/**
 * @brief Keys for SIMLock locks
 */
typedef struct cops_simlock_control_keys {
    /** @brief Network lock control key */
    cops_simlock_control_key_t nl_key;

    /** @brief Network subset lock control key */
    cops_simlock_control_key_t nsl_key;

    /** @brief Service provider lock control key */
    cops_simlock_control_key_t spl_key;

    /** @brief Corporate lock control key */
    cops_simlock_control_key_t cl_key;

    /** @brief Flexible ESL lock control key */
    cops_simlock_control_key_t esll_key;
} cops_simlock_control_keys_t;

/**
 * @brief Status of SIM card as interpreted by SIMLock.
 * NOTE: SIMLock does not require a SIM card to be present.
 * Status can still be approved/not approved depending on the locks set.
 */
typedef enum cops_simlock_card_status {
    /**
     * @brief Status not yet checked.
     *
     * The SIM card has not been checked by the SIMLock yet.
     * (Could be because the user has not entered the SIM PIN
     * yet (if applicable)).
     */
    COPS_SIMLOCK_CARD_STATUS_NOT_CHECKED = 0,

    /**
     * @brief SIM card not approved.
     *
     * The SIM card is not okay with respect to the current
     * SIMLock settings.
     */
    COPS_SIMLOCK_CARD_STATUS_NOT_APPROVED,

    /**
       @brief SIM card approved by SIMLock.
    */
    COPS_SIMLOCK_CARD_STATUS_APPROVED
} cops_simlock_card_status_t;

/** @brief Setting of a SIMLock lock */
typedef enum cops_simlock_lock_setting {
    /**
       @brief The lock is unlocked (inactive).
    */
    COPS_SIMLOCK_LOCK_SETTING_UNLOCKED = 0,

    /**
     * @brief The lock is locked.
     *
     * The lock is locked.
     */
    COPS_SIMLOCK_LOCK_SETTING_LOCKED,

    /** @brief The lock is permanently disabled. */
    COPS_SIMLOCK_LOCK_SETTING_DISABLED,

    /**
     * @brief The lock will be locked to the inserted SIM at next startup.
     *
     * The lock will be locked to the inserted SIM at next
     * startup. In other aspects the lock is not locked.
     * If the SIM is identified as a test SIM the locking is
     * postponed to next startup.
     *
     * This lock setting is only applicable for
     * COPS_SIMLOCK_LOCK_TYPE_NETWORK_LOCK..
     * COPS_SIMLOCK_LOCK_TYPE_SIM_LOCK
     */
    COPS_SIMLOCK_LOCK_SETTING_AUTOLOCK_ENABLED,

    /**
     * @brief The lock will be locked to the CNL of the inserted
     * SIM at next startup.
     *
     * The lock will be locked to the CNL of the inserted SIM at
     * next startup. In other aspects the SIMLock is not locked.
     * If the SIM is identified as a test SIM the locking is
     * postponed to next startup.
     *
     * This lock setting is only applicable for
     * COPS_SIMLOCK_LOCK_TYPE_NETWORK_LOCK..
     * COPS_SIMLOCK_LOCK_TYPE_SIM_LOCK
     */
    COPS_SIMLOCK_LOCK_SETTING_AUTOLOCK_TO_CNL_ENABLED,

    /**
     * @brief Deprecated. Defined for backwards compability only,
     *        Subject for future removal - don't use!
     */
    COPS_SIMLOCK_LOCK_SETTING_AUTO
} cops_simlock_lock_setting_t;

/**
 * @brief Lock definition
 *
 * A bitmask telling which positions in the IMSI and if
 * GID1 and GID2 should be used by the lock in question.
 *
 * \li Bit 0 to 14 indicates position 0 to 14 in the IMSI
 * \li Bit 15 Gid1
 * \li Bit 16 Gid2
 */
typedef uint32_t cops_simlock_lock_definition_t;

/**
 * @brief Defined kind of authentication
 *
 * This type defines what kind of authentication is used when
 * calling privileged function. The values labeled reserved are
 * reserved for future extension and customization.
 */
typedef enum cops_auth_type {
    /**
     * @brief No authentication is provided.
     *
     * No payload within cops_auth_data_t.
     */

    COPS_AUTH_TYPE_NONE = 0,

    /**
     * @brief SIMLock Lock Control Keys are used as authentication.
     *
     * Payload within cops_auth_data_t is cops_simlock_control_keys_t.
     */
    COPS_AUTH_TYPE_SIMLOCK_KEYS,

    /**
     * @brief Supplied to cops_get_challenge for RSA challenge
     *
     * Payload within COPS_AuthenticationData_t the signed challenge as
     * specified in "Platform security" chapter "Authentication".
     */
    COPS_AUTH_TYPE_RSA_CHALLENGE,

    /**
     * @brief Permanent authentication state stored in OTP
     *
     * No payload within cops_auth_data_t.
     */
    COPS_AUTH_TYPE_PERMANENT_AUTHENTICATION,

    /** @brief Reserved */
    COPS_AUTH_TYPE_RESERVED_5,
    /** @brief Reserved */
    COPS_AUTH_TYPE_RESERVED_6,
    /** @brief Reserved */
    COPS_AUTH_TYPE_RESERVED_7,
    /** @brief Reserved */
    COPS_AUTH_TYPE_RESERVED_8,
    /** @brief Reserved */
    COPS_AUTH_TYPE_RESERVED_9,
    /** @brief Reserved */
    COPS_AUTH_TYPE_RESERVED_10,
    /** @brief Reserved */
    COPS_AUTH_TYPE_RESERVED_11,

    /** @brief Master Control Key */
    COPS_AUTH_TYPE_MCK,
    /** @brief Reserved for customer */
    COPS_AUTH_TYPE_CUSTOMER_13,
    /** @brief Reserved for customer */
    COPS_AUTH_TYPE_CUSTOMER_14,
    /** @brief Reserved for customer */
    COPS_AUTH_TYPE_CUSTOMER_15,
    /** @brief Reserved for customer */
    COPS_AUTH_TYPE_CUSTOMER_16,
} cops_auth_type_t;

/**
 * @brief Max size (in bytes) of a challenge response supplied by client
 */
#define COPS_CHALLENGE_RESPONSE_MAX_SIZE (2048)

/**
 * @brief Authentication data
 *
 * The response to a previous challenge presented to cops_get_challenge is
 * located in this structure.
 */
typedef struct cops_auth_data {
    /** @brief The type of challenge response provided. */
    cops_auth_type_t auth_type;

    /** @brief Length a data below. */
    size_t    length;

    /**
     * @brief Authentication data
     *
     * For auth_type = COPS_AUTH_TYPE_RSA_CHALLENGE data is the
     * response to a challenge.
     *
     * For auth_type = COPS_AUTH_TYPE_SIMLOCK_KEYS data is a
     * cops_simlock_control_keys_t.
     *
     * For auth_type = COPS_AUTH_TYPE_PERMANENT_AUTHENTICATION data
     * is set to NULL and length set to 0.
     *
     * For other values of auth_type contents of data is not
     * defined.
     */
    uint8_t  *data;
} cops_auth_data_t;

/** @brief The status of each SIMLock lock */
typedef struct cops_simlock_lock_status {
    /** @brief What type of lock the status refers to. */
    cops_simlock_lock_type_t lock_type;

    /**
     * @brief Definition of the lock.
     *
     * Definition of the lock. Not applicable for
     * Flexible ESL lock.
     */
    cops_simlock_lock_definition_t lock_definition;

    /** @brief The setting of the lock. */
    cops_simlock_lock_setting_t lock_setting;

    /** @brief Attempts left with a SIMLock lock control key. */
    size_t    attempts_left;

    /**
      * @brief TRUE if the timer for this lock is currently
      * running, FALSE otherwise.
      */
    bool      timer_is_running;

    /**
     * @brief The time left on the running timer.
     *
     * The time left on the running timer. Note, this is not an
     * absolute time when the timer expires which means that it will
     * not tell at what time the timer will expire but how much time
     * is left in seconds until the timer will expire.
     */
    uint32_t    time_left_on_running_timer;

    /**
      * @brief The number of attempts with timer delay left.
      *
      * The number of attempts with timer delay left.
      * (Total number of attempts left =
      *  attempts_left + timer_attempts_left)
      */
    size_t    timer_attempts_left;
} cops_simlock_lock_status_t;

/**
 * @brief Status of all SIMLock locks
 *
 * Note that sim_card_status can be
 * COPS_SIMLOCK_CARD_STATUS_NOT_APPROVED but without any locks
 * active. This is the case when integrity of COPS data can not
 * be verified.
 */
typedef struct cops_simlock_status {
    /** @brief Status of the SIM card according to the active SIMLock locks. */
    cops_simlock_card_status_t sim_card_status;

    /** @brief Network lock status. */
    cops_simlock_lock_status_t nl_status;

    /** @brief Network subset lock status. */
    cops_simlock_lock_status_t nsl_status;

    /** @brief Service provider lock status. */
    cops_simlock_lock_status_t spl_status;

    /** @brief Corporate lock status. */
    cops_simlock_lock_status_t cl_status;

    /** @brief (U)SIM Personalization lock status. */
    cops_simlock_lock_status_t siml_status;

    /** @brief Flexible ESL lock status. */
    cops_simlock_lock_status_t esll_status;
} cops_simlock_status_t;

typedef enum cops_device_state {
    COPS_DEVICE_STATE_FULL,
    COPS_DEVICE_STATE_RELAXED
} cops_device_state_t;

typedef enum cops_simlock_ota_commands {
    SIMLOCK_OTA_CURRENTLY_NOT_PERSONALISED = '0',
    SIMLOCK_OTA_PERMANENTLY_NOT_PERSONALISED = '1',
    SIMLOCK_OTA_PERSONALISED = '2',
    SIMLOCK_OTA_IMEI_MISMATCH = '3'
} cops_simlock_ota_commands_t;

struct cops_digest {
    uint8_t value[COPS_DIGEST_LENGTH];
};

/* enum must be 4 bytes in size */
enum cops_payload_type {
    COPS_PAYLOAD_TYPE_MODEM_CODE = 0x07,
    COPS_PAYLOAD_TYPE_GDFS = 0xB,
    COPS_PAYLOAD_TYPE_ITP = 0xD,
    COPS_PAYLOAD_TYPE_UNDEF = 0x7FFFFFFF
};

/* enum must be 4 bytes in size */
enum cops_hash_type {
    COPS_HASH_TYPE_SHA1_HASH   = 0x1,
    COPS_HASH_TYPE_SHA256_HASH = 0x2,
#if 0 /* NOT SUPPORTED */
    COPS_HASH_TYPE_SHA1_HMAC   = 0x10,
    COPS_HASH_TYPE_SHA256_HMAC = 0x20,
#endif
    COPS_HASH_TYPE_UNDEF       = 0x7FFFFFFF
};

/**
 * @brief Return data from the veirfy signed header call
 *
 * @param hdr_size  The size of the signed header
 * @param pl_size   The size of the payload
 * @param ht        The hash type used for calculating the expected hash of the payload
 * @param plt       The payload type protected by the signed header
 * @param ehash     The expected payload hash
 */

struct cops_vsh {
    size_t hdr_size;
    size_t pl_size;
    enum cops_hash_type ht;
    enum cops_payload_type plt;
    struct cops_digest ehash;
};

/**
 * @brief Contains the IMSI (International Mobile Subscriber Identity).
 *
 * @param data Packed IMSI according to 3GPP TS 31.102.
 */
struct cops_simlock_imsi {
    uint8_t data[COPS_PACKED_IMSI_LENGTH];
};

/**
 * @brief The signed header
 *
 * @param magic                 magic number
 * @param size_of_signed_header The size of the signed header
 * @param size_of_signature     The size of the signature (depends on
 *                              signature_type field)
 * @param sign_hash_type        Type of signature hash type
 * @param signature_type        Type of signature
 * @param hash_type             Type of hash
 * @param payload_type          Type of payload
 * @param flags                 Reserved
 * @param size_of_payload       Size of trailing payload
 * @param sw_vers_nbr           Reserved
 * @param load_address          Reserved
 * @param startup_address       Reserved
 * @param spare                 Reserved
 * @param hash                  The actual hash
 * @param signature             the actual signature

 */

struct cops_signed_header {
    uint32_t    magic;
    uint16_t    size_of_signed_header;
    uint16_t    size_of_signature;
    uint32_t    sign_hash_type; /* see t_hash_type */
    uint32_t    signature_type; /* see t_signature_type */
    uint32_t    hash_type;      /* see t_hash_type */
    uint32_t    payload_type;   /* see enum issw_payload_type */
    uint32_t    flags;          /* reserved */
    uint32_t    size_of_payload;
    uint32_t    sw_vers_nbr;
    uint32_t    load_address;
    uint32_t    startup_address;
    uint32_t    spare;          /* reserved */
#if 0
    uint8_t     hash[get_hash_length(this.hash_type)];
    uint8_t     signature[size_of_signature];
#endif
};

/**
 * @brief Type of data in cops_init_arb_table_args_t.
 */
typedef enum cops_arb_data_type {
    COPS_ARB_DATA_TYPE_MODELID
} cops_arb_data_type_t;

/**
 * @brief This type describes what kind of lock that can be set.
 *        The different types of locks all perform a personalization in the end
 *        but they take different paths to do so
 */
typedef enum cops_simlock_lock_mode {
    /**
     * @brief lock mode autolock
     *        Lock to the first SIM that is inserted in the ME when the ME
     *        is restarted. When locking the ME with auto lock the control
     *        keys do not need to be supplied
     */
    COPS_SIMLOCK_LOCK_MODE_AUTOLOCK = 0,

    /**
     * @brief lock mode autolock to CNL
     *        Locks to CNL of the first SIM that is inserted in the ME
     */
    COPS_SIMLOCK_LOCK_MODE_AUTOLOCK_TO_CNL,

    /**
     * @brief lock mode normal lock
     *        Lock to the currently inserted SIM (don't wait for reboot)
     */
    COPS_SIMLOCK_LOCK_MODE_NORMAL_LOCK,

    /**
     * @brief lock mode normal lock to cnl
     *        Locks to the CNL of the currently inserted SIM. If no
     *        CNL exists an error code is returned.
     */
    COPS_SIMLOCK_LOCK_MODE_NORMAL_LOCK_TO_CNL,

    /**
     * @brief lock mode explicit lock IMSI
     *        Lock to the given IMSI (and possible GID fields) provided to
     *        the lock function rather then the inserted SIM.
     */
    COPS_SIMLOCK_LOCK_MODE_EXPLICIT_LOCK_IMSI,

    /**
     * @brief lock mode explicit lock CNL
     *        Lock to the given CNL (and possible GID fields) provided to
     *        the lock function rather then the inserted SIM.
     */
    COPS_SIMLOCK_LOCK_MODE_EXPLICIT_LOCK_CNL,

    /**
     * @brief lock mode explicit lock settings data
     *        Lock to the given lock data provided to the lock function
     *        This can be lock data for any type of lock
     */
    COPS_SIMLOCK_LOCK_MODE_EXPLICIT_LOCK_SETTINGS_DATA,

    /**
     * @brief lock mode reserved
     *        Reserved for future usage.
     */
    COPS_SIMLOCK_LOCK_MODE_RESERVED
} cops_simlock_lock_mode_t;

/**
 * @brief Container for the IMSI
 *
 * @param Data The IMSI formatted according to 3GPP TS 31.102.
 */
typedef struct {
    uint8_t data[COPS_NUM_IMSI_DIGITS];
} __attribute__((__packed__)) cops_simlock_imsi_t;

/**
 * @brief explicit lock data
 *
 * This type defines the data needed when performing a lock operation with
 * COPS_SIMLOCK_LOCK_MODE_EXPLICIT_LOCK_IMSI,
 * COPS_SIMLOCK_LOCK_MODE_EXPLICIT_LOCK_CNL or
 * COPS_SIMLOCK_LOCK_MODE_EXPLICIT_LOCK_SETTINGS_DATA.
 *
 * The data and length parameters are only used for CNL or raw lock data
 *
 * imsi, gid1 and gid2 are only applicable for EXPLICIT_LOCK_IMSI
 * When supplying the IMSI, a complete IMSI of 15 digits isn't needed.
 * Instead it's required that for each bit of the lock definition that is set
 * a valid IMSI digit for that position is provided. Valid IMSI digits are
 * 0 to 9.
 * Example: lock definition for Network lock, 0x1B (binary 1 1 0 1 1)
 * Digits 1, 2, 4 and 5 of IMSI must contain valid IMSI digits (0-9). If not
 * the created lock data is undefined and might not work as expected.
 */
typedef struct cops_simlock_explicitlockdata {
    /** @brief The IMSI to lock the locks against. */
    cops_simlock_imsi_t imsi;

    /** @brief The GID1 used when setting the corporate lock. */
    uint8_t gid1;

    /** @brief The GID2 used when setting the service provider lock. */
    uint8_t gid2;

    /**
     * @brief  The size (in bytes) of the provided data.
     *         When supplying CNL data, the number of CNL rows should be
     *         length / 6
     */
    size_t length;

    /**
     * @brief The provided lock setting data (for CNL or any other lock,
     *        for example flexible ESL)
     *        The client is responsible for allocating this buffer and freeing
     *        it when the function returns.
     */
    uint8_t *data;
} __attribute__((__packed__)) cops_simlock_explicitlockdata_t;

/**
 * @brief simlock lock data
 *
 * This type defines if a lock is to be set and its
 * corresponding control key needed for authentication.
 */
typedef struct cops_simlock_lockdata {
    /**
     * @brief  The type of lock being set.Must be of type enum
     *         cops_simlock_lock_type_t
     */
    uint32_t locktype;

    /**
     * @brief The control key needed to set a lock if not authenticated
     */
    cops_simlock_control_key_t controlkey;

    /**
     * @breif The operation(s) to perform with SIMLock_Lock
     *        Must be of type enum cops_simlock_lock_operation
     */
    uint32_t lockop;

    /**
     * @brief TRUE if the definition of the lock should be updated
     *        with the lockdef below. Unless different definition
     *        from the reasonable default definition already stored
     *        this should normally not be needed.
     */
    bool update_lockdef;

    /**
     * @brief The new definition of the lock. Already existing locks
     *        aren't affected by an update of the definition of the
     *        lock, but if the lock was unlocked and locked again the
     *        new definition could render a different lock.
     */
    cops_simlock_lock_definition_t lockdef;
} __attribute__((__packed__)) cops_simlock_lockdata_t;

/**
 * @brief Definition of the lock argument when setting SIMLocks.
 */
typedef struct cops_simlock_lock_arg {
    /**
     * @brief LockMode           This lock mode describes what kind of lock to
     *                           attempt to set. Must be of type
     *                           cops_simlock_lock_mode_t
     */
    uint32_t lockmode;

    /**
       @brief LockData           Lock data definition.
    */
    cops_simlock_lockdata_t lockdata;

    /**
     * @brief explicitLock data  This data is needed when setting an explicit
     *                           lock, and may be NULL when these modes are set
     *                           COPS_SIMLOCK_LOCK_MODE_AUTOLOCK,
     *                           COPS_SIMLOCK_LOCK_MODE_AUTOLOCK_TO_CNL,
     *                           COPS_SIMLOCK_LOCK_MODE_NORMALLOCK or
     *                           COPS_SIMLOCK_LOCK_MODE_NORMAL_LOCK_TO_CNL
     */
    struct cops_simlock_explicitlockdata *explicitlockdata;
} __attribute__((__packed__))
 cops_simlock_lock_arg_t;

/**
 * @brief Argument for cops_init_arb_table.
 *
 * Data for initalizing anti rollback table
 *
 * Pointers should be initialized to NULL and only point to
 * something to indicate that the parameter is supplied.
 */
typedef struct cops_init_arb_table_args {
    /** @brief Type of data supplied  */
    cops_arb_data_type_t arb_data_type;

    /** @brief Length (in bytes) of following Data */
    size_t    data_length;
    /**
     * @brief Pointer to Data
     *
     * For arb_data_type = COPS_ARB_DATA_TYPE_MODELID the data
     * is a 16 bit modelid
     */
    uint8_t *data;
} cops_init_arb_table_args_t;

/**
 * @brief Type of data in cops_write_secprofile_args_t.
 */
enum cops_sec_profile_data_type {
    COPS_SEC_PROFILE_DATA_NONE
};

/**
 * @brief Argument for cops_write_secprofile.
 *
 * Data for initalizing security data
 *
 * Pointers should be initialized to NULL and only point to
 * something to indicate that the parameter is supplied.
 */
typedef struct cops_write_secprofile_args {
    /** @brief Version */
    uint32_t version;

    /** @brief Flags */
    uint32_t flags;

    /** @brief size of the list */
    size_t hashlist_len;

    /** @brief Pointer to the hashes */
    uint8_t *hashlist;

    /** @brief Type of data supplied  */
    enum cops_sec_profile_data_type sec_profile_data_type;

    /** @brief Length (in bytes) of following Data */
    size_t data_length;

    /** @brief Pointer to Data */
    uint8_t *data;
} cops_write_secprofile_args_t;

/**
 * @brief Type of data in data file.
 */
enum cops_data_type {
    COPS_DATA = 0,
    PERM_AUTH_STATE_DATA
};

#endif /* COPS_TYPES_H */
