/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \brief  PS Connection Control util functions
 */



/********************************************************************************
 * Include Files
 ********************************************************************************/

#include <openssl/md5.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "pscc_utils.h"

/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/

#define PSCC_MIN_CHALLENGE_LENGTH  (32)
#define PSCC_MAX_CHALLENGE_LENGTH  (64)


/********************************************************************************
 *
 * Global variables
 *
 ********************************************************************************/

/********************************************************************************
 *
 * Local variables
 *
 ********************************************************************************/

/********************************************************************************
 *
 * Private function prototypes
 *
 ********************************************************************************/

/*******************************************************************************
 *
 * Public Functions
 *
 *******************************************************************************/

/*
 * pscc_utils_generate_chap_challenge()
 */
size_t pscc_utils_generate_chap_challenge
(
 uint8_t **challenge_pp,
 uint8_t max_challenge_size
)
{
  int i;
  size_t length = 0;
  uint8_t *challenge_p;
  int max_size = 0;

  if (challenge_pp == NULL) {
    return 0;
  }

  // MAL only allows size 50 of max challenge size so it is optional to specify
  if (max_challenge_size > PSCC_MAX_CHALLENGE_LENGTH) {
    max_size = PSCC_MAX_CHALLENGE_LENGTH;
  }
  else if (max_challenge_size < PSCC_MIN_CHALLENGE_LENGTH) {
    max_size = PSCC_MIN_CHALLENGE_LENGTH;
  }
  else {
    max_size = max_challenge_size;
  }

  while(length == 0)
  {/* get non-zero chal_len */
    length = (unsigned)rand() & (max_size-1);   /* get length < MAX */
  }
  while(length < PSCC_MIN_CHALLENGE_LENGTH)
  {/* raise len 'til >= MIN */
    length *= 2;
  }

  /* check that lenght is not greater than max_challenge_size for MAL */
  if (max_challenge_size > 0 && length > max_challenge_size) {
    length = max_challenge_size;
  }

  *challenge_pp = malloc(length);
  if(*challenge_pp == NULL)
    return (0);

  challenge_p = *challenge_pp;

  /* generate a random string valid for CHAP */
  for (i = 0; i < (int)length; i++)
  {
    challenge_p[i] = (uint8_t) (rand() & 0x00ff);
  }
  return (length);
}

/*
 * pscc_utils_calculate_chap_response()
 */
size_t pscc_utils_calculate_chap_response
(
 uint8_t        id,
 const uint8_t* challenge_p,
 size_t         challengelen,
 const char*    secret_p,
 size_t         secretlen,
 uint8_t**      response_pp
)
{
  MD5_CTX * md5context_p;

  if (response_pp == NULL || challenge_p == NULL || secret_p == NULL) {
    return 0;
  }

  *response_pp = malloc(MD5_DIGEST_LENGTH);
  if(*response_pp == NULL)
    return (0);

  md5context_p = malloc(sizeof(MD5_CTX));
  if(md5context_p == NULL)
  {
    free(*response_pp);
    return (0);
  }

  MD5_Init(md5context_p);

  MD5_Update(md5context_p, &id, 1);

  MD5_Update(md5context_p, secret_p, secretlen);

  MD5_Update(md5context_p, challenge_p, challengelen);

  MD5_Final(*response_pp,md5context_p);
  free(md5context_p);
  return (MD5_DIGEST_LENGTH);
}

/*******************************************************************************
 *
 * Private Functions
 *
 *******************************************************************************/

