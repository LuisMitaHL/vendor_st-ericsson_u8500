/*
 * Copyright (C) 2011, ST-Ericsson
 *
 * File name       : util.c
 * Description     : access_sercvices common functionality stub
 *
 * Author          : Carl-Johan Salomonsson <carl-johan.salomonsson@stericsson.com>
 *
 */

#include "util_security.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>

/*******************/
/* Implementation */
/*******************/

util_void_t util_set_restricted_umask()
{
  PRINTDEBUGENTER(util_set_restricted_umask);

  PRINTDEBUGEXIT(util_set_restricted_umask);
}

util_void_t util_continue_as_privileged_user(util_security_cap_t util_cap)
{
    PRINTDEBUGENTER(util_continue_as_privileged_user);
    (void)util_cap;
    PRINTDEBUGEXIT(util_continue_as_privileged_user);
}

int util_read_log_file()
{
  PRINTDEBUGENTER(util_read_log_file);
  PRINTDEBUGEXIT(util_read_log_file);
  return 1;
}

