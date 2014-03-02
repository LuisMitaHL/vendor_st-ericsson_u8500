/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 * 
 * \brief  PS Connection Control Protocol Configuration Option
 */

#ifndef PSCC_PCO_H
#define PSCC_PCO_H

/********************************************************************************
 *
 * Include files
 *
 ********************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "pscc_msg.h"
#include "mpl_list.h"

/********************************************************************************
 *
 * Defines & Type definitions
 *
 ********************************************************************************/

/*******************************************************************************
 *
 * Functions and macros
 *
 *******************************************************************************/
/**
 * pscc_pco_get - 
 *
 * @pco_p    - protocol configuration options
 * @pcolen   - length of protocol configuration options
 *
 * Returns: 
 *
 **/
mpl_list_t *pscc_pco_get
  (
   const uint8_t *pco_p,
   uint8_t pcolen
  );

/**
 * pscc_pco_add_ipcp - 
 *
 * @static_ip  - static ip address
 * @isempty    - adding first pco option
 * @pco_p      - pointer to array to store
 *               protocol configuration options
 * @pcolen     - length of pco array
 *
 * Returns: number of bytes written to pco array, (-1) on error
 *
 **/
int pscc_pco_add_ipcp
  (
   uint32_t  static_ip,
   bool      isempty,
   uint8_t  *pco_p,
   size_t    pcolen
  );

/**
 * pscc_pco_add_auth - 
 *
 * @auth_method  - authentication method
 * @uid_p        - user name (0 terminated string)
 * @pwd_p        - password (0 terminated string)
 * @challenge_p  - chap challenge (valid for auth method chap) 
 * @challengelen - length of chap challenge
 * @response_p   - chap response  (valid for auth method chap) 
 * @responselen  - length of chap response
 * @authid       - protocol id/sequence number
 * @isempty      - adding first pco option
 * @pco_p        - pointer to array to store
 *                 protocol configuration options
 * @pcolen       - length of pco array
 *
 * Returns: number of bytes written to pco array, (-1) on error
 *
 **/
int pscc_pco_add_auth
  (
   pscc_auth_method_t auth_method,
   const char *uid_p,
   const char *pwd_p,
   uint8_t    *challenge_p,
   size_t      challengelen,
   uint8_t    *response_p,
   size_t      responselen,
   uint8_t     authid,
   bool        isempty,
   uint8_t    *pco_p,
   size_t      pcolen
  );

#endif
