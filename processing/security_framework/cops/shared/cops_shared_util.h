/*************************************************************************
 * Copyright ST-Ericsson 2012
 ************************************************************************/
#ifndef COPS_SHARED_UTIL_H
#define COPS_SHARED_UTIL_H

#include <stdint.h>
#include <cops_shared.h>

/**
 * @brief Called to convert IMSI from format specified by 3GPP TS 31.102
 *        to COPS internal format.
 *
 * @param [in]  sim_imsi   Pointer to IMSI formatted according to
 *                          3GPP TS 31.102, COPS_PACKED_IMSI_LENGTH
 * @param [out] cops_imsi  Pointer to IMSI in COPS format, COPS_NUM_IMSI_DIGITS
 *
 * @return cops_return_code_t.
 */
cops_return_code_t cops_util_unpack_imsi(const uint8_t *const sim_imsi,
                                         uint8_t *const cops_imsi);

/**
 * @brief Called to convert IMSI from COPS internal format to
 *        format specified by 3GPP TS 31.102.
 *
 * @param [in]  cops_imsi  Pointer to IMSI in COPS format, COPS_NUM_IMSI_DIGITS
 * @param [out] sim_imsi   Pointer to IMSI formatted according to
 *                          3GPP TS 31.102, COPS_PACKED_IMSI_LENGTH
 *
 * @return cops_return_code_t.
 */
cops_return_code_t cops_util_pack_imsi(const uint8_t *const cops_imsi,
                                       uint8_t *const sim_imsi);

/**
 * @brief Called to convert cops_taf_simlock_status_t to cops_simlock_status_t
 *
 * @param [in]  taf_status Pointer to cops_taf_simlock_status_t
 * @param [out] status     Pointer to cops_simlock_status_t
 *
 * @return cops_return_code_t.
 */
cops_return_code_t
cops_util_convert_status(cops_taf_simlock_status_t *taf_status,
                         cops_simlock_status_t *status);

/**
 * @brief Called to pad all simlock control keys
 *
 * @param [in,out] simlock_keys Pointer to simlock keys struct
 *
 * @return cops_return_code_t.
 */
cops_return_code_t
cops_util_bp_pad_control_keys(cops_simlock_control_keys_t *simlock_keys);

/**
 * @brief Called to pad one simlock control key
 *
 * @param [in,out] key    Pointer to simlock key struct
 * @param [in]     minlen Minimum length of key
 *
 * @return cops_return_code_t.
 */
cops_return_code_t cops_util_bp_pad_control_key(cops_simlock_control_key_t *key,
                                                uint8_t minlen);

#endif /* COPS_SHARED_UTIL_H */
