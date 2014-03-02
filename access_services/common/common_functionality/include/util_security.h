/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __util_security_h__
#define __util_security_h__ (1)

#include <util_general.h>

/**
 * \enum util_security_cap_t
 * \brief Capabilities
 *
 * \n This enum defines the possible capabilities to keep as privileged user.
 */
typedef enum {
    UTIL_SECURITY_CAP_NET_ADMIN = 0x01,     /**< CAP_NET_ADMIN */
    UTIL_SECURITY_CAP_SYS_ADMIN = 0x02      /**< CAP_SYS_ADMIN */
} util_security_cap_t;

/**
 * \fn util_void_t util_continue_as_non_privileged_user(void)
 * \brief Change to a non privileged user.
 *
 * \n This function is intended to be used by the access services servers to switch
 * \n to a non privileged user. Note that this operation applies to each thread
 * \n (threads created after this call will inherit the new user id).

Calling this routine makes it possible to control which
 * \n user id to use in a centralized manner.
 * \n
 * \n After calling this function it is not possible to revert into a privileged state.
 * \n
 *
 * \return                  No return type.
 *
 */
util_void_t util_continue_as_non_privileged_user(void);


/**
 * \fn util_void_t util_continue_as_privileged_user(util_security_cap_t util_cap)
 * \brief Change to a privileged user.
 *
 * \n This function is intended to be used by the access services servers to switch
 * \n to a privileged user. The capability specified is kept. Note that this
 * \n operation applies to each thread (threads created after this call will inherit
 * \n the new user id).
 * \n
 * \n Calling this routine makes it possible to control the needed security in a
 * \n centralized manner.
 * \n
 * \n After calling this function it is not possible to revert into a privileged state.
 * \n
 *
 * \param [in] util_cap     Capabilities to keep.
 *
 * \return                  No return type.
 *
 */
util_void_t util_continue_as_privileged_user(util_security_cap_t util_cap);


/**
 * \fn util_void_t util_set_restricted_umask(void)
 * \brief Set a restricted process umask
 *
 * \n This function sets a restricted process umask. It affects the permission
 * \n of all files created by the process after this calls (e.g. regular files,
 * \n sockets). The chosen mask will give file access only to privileged processes.
 *
 * \return                  No return type.
 *
 */
util_void_t util_set_restricted_umask(void);


#endif /* __util_security_h__ */
