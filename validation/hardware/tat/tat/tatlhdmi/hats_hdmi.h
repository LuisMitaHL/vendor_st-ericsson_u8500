/*****************************************************************************/
/**
*  ST-Ericsson, 2011 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   This module provides services for power services module
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef _HATS_HDMI_H
#define _HATS_HDMI_H

#include <stdint.h>

/**
 * @brief Specifies an aes key.
 */
struct aes_key {
    uint8_t key_data[16]; /** @brief Key bytes */
};

/**
 * @brief Specifies an aes encrypted keyset.
 */
struct hdcp_aes_encrypted_keyset {
    uint8_t ksv[8];      /** @brief Key Selection Vector */
    uint8_t key_data[280]; /** @brief AES encrypted key data. */
    uint32_t key_crc;  /** @brief CRC of the decrypted keys. */
};

/**
 * The hdcp_hats_init() function Initializes AV8100.
 *
 * @return Returns result from init.
 *
 * @retval 0 OK
 * @retval 1 Not OK
 *
 */
int hdcp_hats_init(void);

/**
 * The hdcp_hats_close() function for closing the session.
 *
 * @return Returns result from close.
 *
 * @retval 0 OK
 * @retval 1 Not OK
 *
 */
int hdcp_hats_close(void);


/**
 * The hdcp_hats_check_aes_key_otp() function checks if aes key has been
 * programmed.
 *
 * @return Returns the status of the otp key.
 *
 * @retval 0 if not programmed
 * @retval 1 if programmed
 *
 */
int hdcp_hats_check_aes_key_otp(void);

/**
 * The hdcp_hats_fuse_aes_otp_key() function fuses the aes key.
 * programmed.
 *
 * @param [in] key Pointer to the aes_key struct.
 * @param [in] otp_crc CRC8 value for the key.
 *
 * @return Returns the result of the fuse operation.
 *
 * @retval 0 OK
 * @retval 1 Not OK
 *
 */
int hdcp_hats_fuse_aes_otp_key(struct aes_key *key, uint8_t otp_crc);

/**
 * The hdcp_hats_load_and_verify_aes_hdcp_keys() function loads the keyset
 * and verified a correct CRC.
 *
 * @param [in] keyset Pointer to the hdcp_aes_encrypted_keyset struct.
 *
 * @return
 *
 * @retval 0 OK
 * @retval 1 Not OK
 *
 */
int hdcp_hats_load_and_verify_aes_keys(struct hdcp_aes_encrypted_keyset *keyset);

#endif /* #ifdef _HATS_HDMI_H */
