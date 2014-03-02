/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <sys/stat.h>
#include <util_log.h>
#include <util_security.h>

/*************************************************************************
 *                          UMASK HANDLING                               *
 *************************************************************************
 * The umask is used to specify the default file permissions when a file *
 * is created by a process. In this security context, group membership   *
 * is the way to authenticate a process. Therefore all other file        *
 * permissions are removed. If something else is required, chmod()       *
 * has to be used explicitly to override.                                *
 *                                                                       *
 * The chosen umask 0117 (octal base) will lead to the                   *
 * following permissions:                                                *
 *                                                                       *
 *  0777                                                                 *
 * -0117  which can be expressed as 0777 & ~UMASK = 0060                 *
 * =====                                                                 *
 *  0660 (read/write permission for the owner and group,                 *
 *        disallow the rest)                                             *
 *                                                                       *
 *  where read=4 , write=2, execute=1 is used to describe the            *
 *  file mode.                                                           *
 *                                                                       *
 *  Note that setting this umask will affect socket files as well, thus  *
 *  a non-privileged client cannot connect the socket because there is   *
 *  not enough permissions.                                              *
 *                                                                       *
 *************************************************************************/
#define UMASK 0117

#ifdef ANDROID_BUILD

util_void_t util_set_restricted_umask()
{
    UTIL_LOG_D("setting umask to %04o (%04o permissions)",
               UMASK, 0777 & ~UMASK);
    umask(UMASK);
}

#else /* end of ANDROID_BUILD */

util_void_t util_set_restricted_umask()
{
    UTIL_LOG_D("module test environment, not changing umask");
}

#endif /* non Android build */
