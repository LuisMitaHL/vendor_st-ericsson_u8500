/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief  PS Connection Control Utils
 */

#ifndef PSCC_UTILS_H
#define PSCC_UTILS_H

#include <stdint.h>
#include <stdbool.h>

/**
 * pscc_utils_generate_chap_challenge -
 *
 * @challenge_pp    - pointer to generated challenge
 * @max_challenge_size - max size of the challenge, use 0 for default value
 *
 * Returns: length of generated challenge
 *
 **/
size_t pscc_utils_generate_chap_challenge
(
 uint8_t **challenge_pp,
 uint8_t max_challenge_size
);

/**
 * pscc_utils_calculate_chap_response -
 *
 * @id
 * @challenge_p    - pointer to chap challenge
 * @challengelen   - length of chap challenge
 * @secret_p       - pointer to secret
 * @secretlen      - length of secret
 * @response_pp    - pointer to calculated response
 *
 * Returns: length of generated chap response
 *
 **/
size_t pscc_utils_calculate_chap_response
(
 uint8_t        id,
 const uint8_t* challenge_p,
 size_t         challengelen,
 const char*    secret_p,
 size_t         secretlen,
 uint8_t**      response_pp
);

#endif //PSCC_UTILS_H
