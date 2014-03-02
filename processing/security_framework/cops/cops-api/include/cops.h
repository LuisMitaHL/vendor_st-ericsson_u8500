/*************************************************************************
 * Copyright ST-Ericsson 2010
 ************************************************************************/
#ifndef COPS_H
#define COPS_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
/* Due to OPA file name collision we need this */
#ifdef COPS_USE_AUTO_GEN_INCLUDE
#include "cops_types_auto_gen.h"
#else
#include <cops_types.h>
#endif

/**
 * @brief Represents a communication channel with the COPS daemon
 *
 * All communication with the COPS daemon is done through a COPS
 * context.
 */
typedef struct cops_context_id cops_context_id_t;



/**
 * @brief Information about the locks that were unlocked Over The
 * Air (OTA). Set to true if a lock is unlocked. (If a lock was
 * previously unlocked the flag will be true for that lock).
 */
typedef struct cops_simlock_ota_unlock_status {
    /** @brief ture if network lock has been unlocked. */
    bool      network;
    /** @brief ture if network subset lock has been unlocked. */
    bool      network_subset;
    /** @brief ture if service provider lock has been unlocked. */
    bool      service_provider;
    /** @brief ture if corporate lock has been unlocked. */
    bool      corporate;
} cops_simlock_ota_unlock_status_t;

/**
 * @brief status of the sim card and what lock that failed (if the
 * card is not approved)
 */
typedef struct cops_simcard_status {
    cops_simlock_card_status_t card_status;
    cops_simlock_lock_type_t failed_lt;
} cops_sim_status_t;

/**
 * @brief Callback functions for COPS events
 *
 * These functions will be called from
 * cops_context_invoke_callback() if not NULL when the
 * corresponding event has occured.
 */
typedef struct cops_event_callbacks {
    /**
     * @brief Called when OTA unlock has been performed
     *
     * @param [in] event_aux        Auxiliary pointer for events
     * @param [in] unlock_status    Pointer to
     */
    void (*ota_unlock_status_updated_cb)(void *event_aux,
                                         const
                                         cops_simlock_ota_unlock_status_t
                                         *unlock_status);

    /**
     * @brief Called when SIMLock lock status has changed for the SIM card
     *
     * For the SIM card to be approved for radio usage sim_card_status
     * has to be COPS_SIMLOCK_CARD_STATUS_APPROVED.
     *
     * @param [in] event_aux        Auxiliary pointer for events
     * @param [in] status           Status of SIM card according to
     *                              current SIMLock settings
     *
     * @return Returns void.
     */
    void (*simlock_status_changed_cb)(void *event_aux,
                                      const cops_sim_status_t
                                      *status);

    /**
     * @brief Called when (de)authentication has been performed
     *
     * @param [in] event_aux        Auxiliary pointer for events
     */
    void (*authentication_changed_cb)(void *event_aux);
} cops_event_callbacks_t;

/**
 * @brief Callback functions for asyncronous COPS calls
 *
 * These functions will be called from
 * cops_context_invoke_callback() if not NULL when the
 * corresponding call is completed.
 */
typedef struct cops_async_callbacks {

    /**
     * @brief Called when an asynchronous call to cops_read_imei() has
     *        completed.
     *
     * @param [in] async_aux        Auxilary pointer for this call
     * @param [in] rc               Return code of the call
     * @param [in] imei             Pointer to the struct containing the IMEI.
     *                              Garbage if rc != COPS_RC_OK.
     *
     * @return Returns void.
     */
    void (*read_imei_cb)(void *async_aux, cops_return_code_t rc,
                         const cops_imei_t *imei);
} cops_async_callbacks_t;

/**
 * @brief Creates a new communication channel with the COPS daemon
 *
 * All communication with the COPS daemon is done using a context
 * ID. The context ID hides the underlaying socket used for the
 * communication. One exception is the file descriptor for the
 * client to wait for events with.
 *
 * The returned context ID is later freed with cops_context_destroy()
 * when not needed any longer.
 *
 * Events are delivered to the client through supplied one
 * callback function for each type of event.  The client should
 * only supply pointers to callback function for events it is
 * interested in receiving. For example, very few clients are
 * interesed in the event "OTP unlock status updated" and would
 * thus set ota_unlock_status_updated_cb to NULL.
 *
 * The callback functions are called when cops_context_invoke_callback()
 * is called by the client.
 *
 * @param  [in,out] ctxpp Pointer to a pointer to the context ID.
 *                        *ctxpp has to be set to NULL when the
 *                        function is called or an error will be
 *                        returned.
 * @param [in] event_cbs  Pointer to a callback functions to be called
 *                        when different events occurs.
 * @param [in] event_aux  Auxilary pointer supplied to callback functions.
 *
 * @return cops_return_code_t
 * (see definition of cops_return_code_t for description about return codes)
 */
cops_return_code_t cops_context_create(cops_context_id_t **ctxpp,
                                       const cops_event_callbacks_t *event_cbs,
                                       void *event_aux);

/**
 * @brief Creates a new asyncronous communication channel with
 * the COPS daemon
 *
 * All communication with the COPS daemon is done using a context
 * ID. The context ID hides the underlaying socket used for the
 * communication. One exception is the file descriptor for the
 * client to wait for something to happen with.
 *
 * The returned context ID is later freed with cops_context_destroy()
 * when not needed any longer.
 *
 * Events are delivered to the client through supplied callback functions,
 * one callback function for each type of event.  The client
 * should only supply pointers to callback function for events it
 * is interested in receiving. For example, very few clients are
 * interesed in the event "OTP unlock status updated" and would
 * thus set ota_unlock_status_updated_cb to NULL.
 *
 * The result of asyncronous calls to COPS is delivered to the client
 * through supplied callback functions, one callback function for each
 * type of call. The client need only supply pointer to callback function
 * for functions it intend to call. If a function without a registered
 * callback function is called an error will be returned.
 *
 * If the client requires both syncronous and asyncronous calls to COPS
 * two context IDs should be created, one for syncronous calls and one
 * for asyncronous calls.
 *
 * The callback functions are called when cops_context_invoke_callback()
 * is called by the client.
 *
 * @param  [in,out] ctxpp Pointer to a pointer to the context ID.
 *                        *ctxpp has to be set to NULL when the
 *                        function is called or an error will be
 *                        returned.
 * @param [in] event_cbs  Pointer to a callback functions to be called
 *                        when different events occurs.
 * @param [in] event_aux  Auxilary pointer supplied to events callback
 *                        functions.
 * @param [in] async_cbs  Pointer to a callback functions to be called
 *                        when an asyncronous call has completed.
 * @param [in] async_aux  Auxilary pointer supplied to asyncronous callback
 *                        functions. This pointer can later be updated with
 *                        cops_context_set_async_aux().
 *
 * @return cops_return_code_t
 * (see definition of cops_return_code_t for description about return codes)
 */
cops_return_code_t cops_context_create_async(cops_context_id_t **ctxpp,
        const cops_event_callbacks_t *event_cbs,
        void *event_aux,
        const cops_async_callbacks_t *async_cbs,
        void *async_aux);

/**
 * @brief Destroys an established communication with the COPS daemon.
 *
 * This function frees all resources linked to the context ID and
 * sets *ctxpp to NULL before returning.
 *
 * @param [in,out]  ctxpp   *ctxpp may already be set to NULL in which case
 *                          nothing is done.
 */
void      cops_context_destroy(cops_context_id_t **ctxpp);

/**
 * @brief Sets a new auxilary pointer to be used in subsequent calls.
 *
 * Note that already started calls will get the previous async_aux
 * as arguemtn.
 *
 * @param [in,out] ctxp
 * @param [in]     async_aux
 *
 * @return Returns void
 */
void      cops_context_set_async_aux(cops_context_id_t *ctxp, void *async_aux);

/**
 * @brief Get file descriptor of the asyncronous socket use to deliver
 * events and asyncronous completion of calls.
 *
 * This function returns the file descriptor of the event socket. This
 * file descriptor can be used to wait for something to happen. The
 * file descriptor should not be read from directly as that may break
 * the protocol used on the socket.
 *
 * @param [in] ctxp Context ID
 * @param [out] fdp Pointer to file descriptor
 *
 * @return cops_return_code_t.
 * (see definition of cops_return_code_t for description about return codes)
 */
cops_return_code_t cops_context_async_fd_get(cops_context_id_t *ctxp, int *fdp);

/**
 * @brief Tells if an event or asyncronous completion of a call is pending
 *
 * This function tells if there are any events or asyncronous
 * completion of a call pending on a context ID
 *
 * @param [in] ctxp Pointer to context ID
 *
 * @return Returns TRUE if an event is pending, FALSE otherwise.
 */
bool      cops_context_has_async(cops_context_id_t *ctxp);

/**
 * @brief Delivers events and asyncronous completion to
 * registered callback functions.
 *
 * This function deliver events to registered callback functions.
 *
 * @param [in] ctxp Pointer to context ID
 *
 * @return cops_return_code_t.
 * (see definition of cops_return_code_t for description about return codes)
 */
cops_return_code_t cops_context_invoke_callback(cops_context_id_t *ctxp);

/**
 * @brief Read IMEI.
 *
 * This function returns the IMEI stored in secured data of the ME.
 *
 * This function supports asyncronous completion if a callback function
 * is registered with cops_context_create_async().
 *
 * @param [in] ctxp  Pointer to context ID
 * @param [out] imei Pointer to the struct containing the IMEI. The pointer
 *                   may be NULL when the function is called in asyncronous
 *                   mode.
 *
 * @return cops_return_code_t.
 * (see definition of cops_return_code_t for description about return codes)
 */
cops_return_code_t cops_read_imei(cops_context_id_t *ctxp, cops_imei_t *imei);

/**
 * @brief Get number of OTP rows.
 *
 * This function returns the number of OTP rows.
 *
 * @param [in]      ctxp Pointer to context ID
 * @param [out]     nbr_of_otp_rows, pointer to the result.
 *
 * @return cops_return_code_t.
 * (see definition of cops_return_code_t for description about return codes)
 */
cops_return_code_t cops_get_nbr_of_otp_rows(cops_context_id_t *ctxp,
                                            uint32_t *nbr_of_otp_rows);

/**
 * @brief Read OTP.
 *
 * This function returns the OTP of the ME.
 * Data and meta parameters such as ECC and lock bits are returned in the
 * in-out buffer.
 * If any meta read lock bits are set, corresponding data is invalid.
 * For details on meta bits, see cops_write_otp()
 *
 * @param [in]      ctxp Pointer to context ID
 * @param [in, out] buffer Pointer to OTP data.
 * @param [in]      size Number of bytes in the reserved buffer.
 *
 * @return cops_return_code_t.
 * (see definition of cops_return_code_t for description about return codes)
 */
cops_return_code_t cops_read_otp(cops_context_id_t *ctxp, uint32_t *buffer,
                                 size_t size);

/**
 * @Brief Write OTP
 * OTP data consists of two uint32_t for each row. One with meta information
 * and one with data to be written. Meta bits are as follows:
 * Name                                  Bit position
 * OTP_META_ECC_BIT                      31
 * OTP_META_PERMANENT_WRITE_LOCK_BIT     30
 * OTP_META_PERMANENT_READ_LOCK_BIT      29
 * OTP_META_STICKY_WRITE_LOCK_BIT        28
 * OTP_META_STICKY_READ_LOCK_BIT         27
 * OTP_META_AUTH_WRITE_LOCK_BIT          26
 * OTP_META_AUTH_READ_LOCK_BIT           25
 * OTP_META_NON_AUTH_WRITE_LOCK_BIT      24
 * OTP_META_NON_AUTH_READ_LOCK_BIT       23
 * OTP_META_ROW_NUMBER                   15 - 0
 *
 * No write of OPT is performed unless input data is tested OK.
 *
 * @param [in]      ctxp Pointer to context ID
 * @param [in, out] buffer Pointer to OTP data.
 * @param [in]      size Number of bytes in the buffer.
 *
 * @return cops_return_code_t.
 * (see definition of cops_return_code_t for description about return codes)
 */
cops_return_code_t cops_write_otp(cops_context_id_t *ctxp,
                                  uint32_t *buffer, size_t size);

/**
 * @brief Read COPS secured data.
 *
 * This function reads parameter data from secured data. The
 * parameter data is always returned regardless of the integrity.
 *
 * If the function fails with error code COPS_RC_DATA_TAMPERED_ERROR
 * the integrity could not be verified, but the data is still returned.
 *
 * @param [in] ctxp     Pointer to context ID.
 * @param [out] buf     The buffer for reading data.
 *                      NULL to query required length.
 * @param [in,out] blen Pointer to the length (in bytes) of the data buffer.
 *                      Updated with actual length of data on return.
 *
 * @return cops_return_code_t.
 * (see definition of cops_return_code_t for description about return codes)
 */
cops_return_code_t cops_read_data(cops_context_id_t *ctxp,
                                  uint8_t *buf, size_t *blen);

/**
 * @brief Argument for cops_bind_properties().
 *
 * This type defines the different parts of secured data that the
 * BindProperties function can change.
 *
 * Pointers should initialized to NULL and only point to
 * something to indicate that the parameter is supplied.
 *
 * New authentication data are for example new control keys.
 */
typedef struct cops_bind_properties_arg {
    /** @brief Not NULL if a new IMEI is supplied. */
    const cops_imei_t *imei;

    /** @brief Number of following new authentication data. */
    size_t    num_new_auth_data;
    /**
     * @brief Array of new authentication data, may be NULL if
     * num_new_auth_data is 0. The auth data can for example be
     * new SIMLock lock control keys.
     */
    const cops_auth_data_t *auth_data;

    /** @brief Length (in bytes) of following COPS Data */
    size_t    cops_data_length;
    /** @brief Pointer to COPS Data **/
    const uint8_t *cops_data;
    /**
     * @brief True if COPS Data above should be merged with
     * previously stored COPS Data.
     *
     * True if COPS Data above should be merged with previously
     * stored COPS Data or false if it should be overwritten.
     *
     * If COPS Data is merged and there is a conflict with
     * already present COPS Data the supplied COPS Data takes
     * precedence.
     */
    bool      merge_cops_data;
} cops_bind_properties_arg_t;

/**
 * @brief Bind Properties
 *
 * When the ME is produced a couple of steps are commonly
 * performed (not necessarily in this order).
 *
 * 1. Write COPS data.
 * 2. Write IMEI to COPS data.
 * 3. Write new authentication data e.g. control keys to COPS data.
 * 4. Bind properties. This means that the COPS data will be integrity
 *    protected to avoid unauthorized modification.
 *    The control key protection has to be integrity protected in order to
 *    start the radio and hence needs to be verified every time a client is
 *    communicating with COPS.
 *
 * Because this function is fundamental to the security solution,
 * it requires authentication. Depending on the current state
 * of the data a certain level may be necessary.
 *
 * The authentication is performed by calling cops_authenticate()
 * before calling cops_bind_properties(). The authentication
 * itself can be performed by different items, e.g. SimLock
 * ControlKeys, RSA authentication etc.
 *
 * The SIMLock lock control keys and other authentication data can
 * also be changed or initially stored by this function.
 *
 * Notes:
 * - It is always necessary to authenticate prior to this function.
 * - Depending on the current state of the data the function may fail if either
 *   the data are not correct or if the level of authentication is not
 *   sufficient. Please take a look to the log in that case.
 *
 * @param [in] ctxp     Pointer to context ID.
 * @param [in] arg      Pointer to new security data to be stored.
 *
 * @return cops_return_code_t.
 * (see definition of cops_return_code_t for description about return codes)
 */
cops_return_code_t cops_bind_properties(cops_context_id_t *ctxp,
                                        cops_bind_properties_arg_t *arg);

/**
 * @brief Get a challenge for authentication.
 *
 * Get a challenge from COPS. The caller will then sign the
 * challenge and use it as authentication when calling
 * BindProperties.
 *
 * @param [in] ctxp         Pointer to context ID.
 * @param [in] auth_type    Type of authentication.
 * @param [out] buf         The buffer to stored challenge data.
 *                          NULL to query required length of buffer.
 * @param [in,out] blen     Pointer to the length (in bytes) of the challenge
 *                          buffer. Updated with actual length of challenge
 *
 * @return cops_return_code_t.
 * (see definition of cops_return_code_t for description about return codes)
 */
cops_return_code_t cops_get_challenge(cops_context_id_t *ctxp,
                                      cops_auth_type_t auth_type,
                                      uint8_t *buf, size_t *blen);

/**
 * @brief Authenticate for operations requiring authentication.
 *
 * This functions puts the ME in an authenticated state for
 * operations requiring authentication.
 *
 * The authentication data supplied is either all the five SIMLock
 * lock control keys, a signed challenge that COPS has issued
 * and the caller retrieved prior to calling this function or the
 * permanent authentication field from OTP. If the latter is used,
 * COPS will become authenticated if the flag is set in OTP.
 *
 * @param [in] ctxp         Pointer to context ID.
 * @param [in] permanently  If true permanently authenticate.
 *                          If false authenticate until next
 *                          reboot of the ME or deauthenticate called.
 * @param [in] auth_data    Pointer to authentication data.
 *
 * @return cops_return_code_t.
 * (see definition of cops_return_code_t for description about return codes)
 */
cops_return_code_t cops_authenticate(cops_context_id_t *ctxp, bool permanently,
                                     cops_auth_data_t *auth_data);

/**
 * @brief Deauthenticate from prior authentications.
 *
 * This function deauthenticates the ME from prior
 * authentications to prevent unauthorized access.
 *
 * @param [in] ctxp         Pointer to context ID.
 * @param [in] permanently  If true permanently deauthenticate.
 *                          If false deauthenticate until next
 *                          reboot of the ME.
 *
 * @return cops_return_code_t.
 * (see definition of cops_return_code_t for description about return codes)
 */
cops_return_code_t cops_deauthenticate(cops_context_id_t *ctxp,
                                       bool permanently);

/**
 * @brief Unlock a SIMLock lock
 *
 * Unlocks the specified lock if the correct control key is
 * supplied or if already authenticated.
 *
 * This function only supports SIMLock_LockType_t in the range from
 * SIMLOCK_LOCK_TYPE_NETWORK_LOCK to SIMLOCK_LOCK_TYPE_FLEXIBLE_ESL_LOCK.
 *
 * @param [in] ctxp         Pointer to context ID.
 * @param [in] lock_type    The type of lock to unlock
 * @param [in] control_key  If not NULL, pointer to a control key
 *                          for authentication when attempting to
 *                          unlock a lock.
 *
 * @return cops_return_code_t.
 * (see definition of cops_return_code_t for description about return codes)
 */
cops_return_code_t cops_simlock_unlock(cops_context_id_t *ctxp,
                                       cops_simlock_lock_type_t lock_type,
                                       cops_simlock_control_key_t *control_key);

/**
 * @brief Lock specified SIMLock lock.
 *
 * Locks SIMLock locks in the ME in five different ways.
 *
 * If the supplied lockmode is set to COPS_SIMLOCK_LOCK_MODE_AUTOLOCK
 * the following applies:
 * The next time the ME restarts with a SIM inserted, the ME will attempt
 * to lock the supplied SIMLock against the SIM. If successful, the bits
 * will then be cleared, avoiding the locks from being set yet
 * again at the next reboot (with a possibly different SIM).
 *
 * Exceptions:
 * If the SIM card is a test SIM, no actions are taken.
 *
 * If the supplied lockmode is set to
 * COPS_SIMLOCK_LOCK_MODE_AUTOLOCK_TO_CNL the functionality will be
 * very similar to the AUTOLOCK case, except the ME will be
 * locked to the CNL on the SIM rather than just the IMSI.
 *
 * Exceptions:
 * If the SIM card does not have a CNL or the list is empty, the
 * flag will remain until a SIM with a CNL is inserted and the ME
 * rebooted.
 *
 * If the supplied lockmode is set to COPS_SIMLOCK_LOCK_MODE_NORMAL_LOCK
 * the following applies:
 * If the supplied control key is correct the device will be SIMLocked
 * to the currently inserted SIM card. Note, it will not lock to Flexible ESL.
 * This can only be done when calling with the lock type
 * COPS_SIMLOCK_LOCK_MODE_EXPLICIT_LOCK_FLEXIBLE_ESL.
 *
 * Exceptions:
 * If there is no SIM in the ME, an error code will be returned
 * and no changes to the SIMLock performed.
 *
 * If the supplied lockmode is set to
 * COPS_SIMLOCK_LOCK_MODE_NORMAL_LOCK_TO_CNL the same as above occurs
 * except the ME will lock to the CNL of the SIM rather than the
 * IMSI.
 *
 * Exceptions:
 * If the SIM card lacks a CNL or no SIM card is in place the
 * function will fail.
 *
 * If the supplied lockmode is set to
 * COPS_SIMLOCK_LOCK_MODE_EXPLICIT_LOCK_IMSI,
 * COPS_SIMLOCK_LOCK_MODE_EXPLICIT_LOCK_CNL or
 * COPS_SIMLOCK_LOCK_MODE_EXPLICIT_LOCK_SETTINGS_DATA the following applies:
 * If the supplied control key is correct it will be SIMLocked to the
 * data supplied as input to the function. Only this lockmode will reference
 * the explicitlockdata member in the cops_simlock_lock_arg type.
 * If flexible_esl lock is set to be locked, the explicitlockdata
 * must be filled with the lock data and the appropriate length of it set.
 *
 * This function only supports simlock_lock_type_t in the range from
 * SIMLOCK_TYPE_NETWORK to SIMLOCK_TYPE_FLEXIBLE_ESL.
 *
 * Note this function only accepts one lock to be set at time. Call this
 * function multiple times to set more than one lock.
 *
 * @param [in] lockarg    A pointer to the struct containing the input
 *                        necessary to lock the ME.
 *                        Must not be NULL.
 *
 * @return cops_return_code_t.
 */
cops_return_code_t cops_simlock_lock(cops_context_id_t *ctxp,
                                     struct cops_simlock_lock_arg *lockarg);

/**
 * @brief Verify control keys
 *
 * Verify if all the supplied Control Keys are the same as the
 * ones stored in the ME. Note that the SIM Personalization control key
 * is ignored during this verification.
 *
 * @param [in] ctxp         Pointer to context ID.
 * @param [in] control_keys Pointer to control keys
 *
 * @return cops_return_code_t.
 * (see definition of cops_return_code_t for description about return codes)
 */
cops_return_code_t cops_simlock_verify_control_keys(cops_context_id_t *ctxp,
                                    cops_simlock_control_keys_t *control_keys);

/**
 * @brief Get SIMLock status
 *
 * Reports the status of the locks in the ME.
 *
 * @param [in] ctxp         Pointer to context ID.
 * @param [out] simlock_status Pointer to where to return status.
 *
 * @return cops_return_code_t.
 * (see definition of cops_return_code_t for description about return codes)
 */
cops_return_code_t cops_simlock_get_status(cops_context_id_t *ctxp,
                                         cops_simlock_status_t *simlock_status);

/**
 * @brief Change SIM/USIM Personalization control key
 *
 * Changes the control key if the old supplied one is
 * correct. If the lock is disabled the function will return an error.
 *
 * @param [in] ctxp         Pointer to context ID.
 * @param [in] old_control_key Pointer the old control key
 * @param [in] new_control_key Pointer the new control key
 *
 * @return cops_return_code_t
 * (see definition of cops_return_code_t for description about return codes)
 */
cops_return_code_t cops_simlock_change_sim_control_key(cops_context_id_t *ctxp,
                                   cops_simlock_control_key_t *old_control_key,
                                   cops_simlock_control_key_t *new_control_key);

/**
 * @brief The max size (in bytes) of the OTA message containing
 * unlock codes for the SIMLock locks.
 */
#define COPS_SIMLOCK_OTA_MESSAGE_MAX_SIZE       (120)

/**
 * @brief The size (in bytes) of the response message after
 * attempting to depersonalize a ME.
 */
#define COPS_SIMLOCK_OTA_REPLY_MESSAGE_SIZE (20)

/**
 * @brief Contains the response message to the network after an
 * over the air (OTA) depersonalisation has been performed.
 */
typedef struct cops_simlock_ota_reply_message {
    /** @brief The response message after attempting to depersonalize the ME. */
    uint8_t   data[COPS_SIMLOCK_OTA_REPLY_MESSAGE_SIZE];
} cops_simlock_ota_reply_message_t;

/**
 * @brief SIMLock Over the air Unlock Function for unlocking the ME over
 * the air (OTA), by sending the ME a SMS with the IMEI of the ME
 * and the control keys for unlocking it.
 *
 * Unlocks the specified locks if the Control Keys are correct.
 * This request must only be used by SMS service. The message
 * syntax is controlled by this function but the OTA interface
 * must be handled by the calling process, this includes
 * transforming the data to ASCII.
 *
 * @param [in] buf      The buffer holding the input message with the
 *                      control keys for unlocking the ME.
 * @param [in] blen     Length of the buffer above, max length is
                        COPS_SIMLOCK_OTA_MESSAGE_MAX_SIZE.
 * @param [out] reply   The reply message to send back to the network
 *                      with the unlocking status.
 *
 * @return cops_return_code_t.
 * (see definition of cops_return_code_t for description about return codes)
 */
cops_return_code_t cops_simlock_ota_unlock(cops_context_id_t *ctxp,
        const uint8_t *buf,
        size_t blen,
        cops_simlock_ota_reply_message_t
        *reply);

/**
 * @brief Finds a parameter in COPS secured data.
 *
 * This function finds a parameter in COPS secured data obtained with
 * cops_read_data(). The primary use of this function is to access
 * customer defined data that is stored in COPS secured data.
 *
 * @param [in]  buf         COPS data buffer
 * @param [in]  blen        Length of COPS data buffer above
 * @param [in]  param_id    Paramter ID to find
 * @param [out] param_data  Pointer to pointer where the address of the
 *                          parameter data inside the COPS data buffer is
 *                          returned
 * @param [out] param_len   Length of the data above is retured through
 *                          this pointer.
 *
 * @return cops_return_code_t
 * (see definition of cops_return_code_t for description about return codes)
 */
cops_return_code_t cops_util_find_parameter(uint8_t *buf, size_t blen,
        int param_id, uint8_t **param_data,
        size_t *param_len);

/**
 * @brief Get device state.
 *
 * @param [in] ctxp  Pointer to context ID.
 * @param [out] device_state Pointer to the device state.
 *
 * @return cops_return_code_t.
 */
cops_return_code_t cops_get_device_state(cops_context_id_t *ctxp,
        cops_device_state_t *device_state);

/**
 * This function calculates an ME unique MAC for supplied data.
 *
 * @param [in]  ctxp              Pointer to context ID.
 * @param [in]  buf               Pointer to data.
 * @param [in]  blen              Length (in bytes) of data above.
 * @param [out] mac               Pointer to calculated MAC data.
 *
 * @return cops_return_code_t
 *
 * @note ITP is not allowed to call this function without being authenticated,
 *       or it will fail. If some other party calls the function
 *       (on the access or data cpu) the function will not require a prior
 *       authentication.
 */
cops_return_code_t cops_bind_data(cops_context_id_t *ctxp,
                                  const uint8_t *buf, uint32_t blen,
                                  const struct cops_digest *mac);

/**
 * This function verifies that the supplied data matches the ME unique MAC.
 *
 * @param [in] ctxp            Pointer to context ID.
 * @param [in] buf             Pointer to data.
 * @param [in] blen            Length (in bytes) of data above.
 * @param [in] mac             Pointer to MAC of data above.
 *
 * @return cops_return_code_t.
 */
cops_return_code_t cops_verify_data_binding(cops_context_id_t *ctxp,
        const uint8_t *buf,
        uint32_t blen,
        const struct cops_digest *mac);

/**
 * This function verifies a signed header.
 *
 * @param [in]  ctxp          Pointer to context ID.
 * @param [in]  header        Pointer to signed header to verify and
 *                            return the expected hash from.
 *                            Must not be NULL.
 * @param [in]  pltype        Type of payload.
 * @param [out] hinfo         The client should calculate a hash
 *                            over the payload accompanying the
 *                            signed header and compare to the
 *                            expected hash included in this out param.
 *                            Must not be NULL.
 *
 * @return cops_return_code_t
 */

cops_return_code_t cops_verify_signed_header(cops_context_id_t *ctxp,
        const uint8_t *header,
        enum cops_payload_type pltype,
        struct cops_vsh *hinfo);

/**
 * Calculates a hash over the data pointed out by the buffer
 * and length parameters according to the hash type parameter.
 * The data must reside in consecutive chunks in memory or the
 * function will produce an incorrect hash (i.e. the function
 * will not be able to follow pages out-of-order).
 *
 * @param [in]  ctxp        Pointer to context ID.
 * @param [in]  ht          The hash type to use.
 * @param [in]  data        Pointer to data to calculate the hash from.
 *                          Must not be NULL.
 * @param [in]  datalen     The length of the data above.
 * @param [out] hash        The calculated hash.
 *                          Must not be NULL.
 *
 * @return cops_return_code_t
 */

cops_return_code_t cops_calcdigest(cops_context_id_t *ctxp,
                                   enum cops_hash_type ht,
                                   const uint8_t *data,
                                   size_t datalen,
                                   struct cops_digest *hash);

cops_return_code_t cops_simlock_verify_imsi(cops_context_id_t *ctxp,
        const struct cops_simlock_imsi *imsi);

/**
 * @brief Locks the bootpartition.
 *
 * NOTE: this function locks the bootpartition (which is a permanent change)
 * without checking if the user is authenticated or not
 *
 * @param [in] ctxp  Pointer to context ID.
 *
 * @return cops_return_code_t.
 */
cops_return_code_t cops_lock_bootpartition(cops_context_id_t *ctxp);

/**
 * @brief Initialize an empty sw version table
 *
 * @param [in] ctxp     Pointer to context ID.
 * @param [in] arg      Pointer to arguments for initializing arb table.
 *
 * @return cops_return_code_t.
 */
cops_return_code_t cops_init_arb_table(cops_context_id_t *ctxp,
                                       cops_init_arb_table_args_t *arg);

/**
 * @brief Write security data to bootpartition.
 *
 * @param [in] ctxp     Pointer to context ID.
 * @param [in] arg      Pointer to arguments to initalizing security data.
 *
 * @return cops_return_code_t.
 */
cops_return_code_t cops_write_secprofile(cops_context_id_t *ctxp,
                                         cops_write_secprofile_args_t *arg);

/**
 * @brief Write RPMB key.
 *
 * The RPMB key will be generated by secure world based on chip-unique key.
 * And this function will call into secure world to first generate the key
 * and then write it to RPMB through RPMB API in secure world.
 *
 * @param [in] ctxp        Pointer to context ID
 * @param [in] dev_id      The device ID of RPMB device
 * @param [in] commercial  Flag to indicate if commercial or development
 *                         key should be used.
 *
 * @return cops_return_code_t.
 */
cops_return_code_t cops_write_rpmb_key(cops_context_id_t *ctxp,
                                       uint16_t dev_id,
                                       bool commercial);

/**
 * @brief Bit-masks for product debug settings bit field.
 *
 *    - COPS_FLAGS_JTAG_ENABLED_MODEM
 *    - COPS_FLAGS_JTAG_ENABLED_APE
 *
 *    u8500      : Modem = Renesas,      APE = APE
 *    Thorium    : Modem = CPU_A/CPU_B,  APE = DCPU
 *    9540 (APE) : Modem = N/A,          APE = APE
 */
#define COPS_FLAG_JTAG_ENABLED_MODEM  0x00000002
#define COPS_FLAG_JTAG_ENABLED_APE    0x00000008

/**
 * @brief Get product debug settings.
 *
 * @param [in]  ctxp               Pointer to context ID.
 * @param [out] debug_settings     Pointer to debug settings.
 *
 * Use the JTAG flags above to mask out the settings.
 *
 * @return cops_return_code_t.
 */
cops_return_code_t cops_get_product_debug_settings(cops_context_id_t *ctxp,
                                                   uint32_t *debug_settings);

#endif                          /* COPS_H */
