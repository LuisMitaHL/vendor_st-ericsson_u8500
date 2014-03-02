/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <stdio.h>
#include <unistd.h>

#include <linux/capability.h>
#include <linux/prctl.h>

#include <util_log.h>
#include <util_security.h>

#ifdef ANDROID_BUILD

#include <android_filesystem_config.h>

/* For the access services servers the radio user is the one to use.
 * The uid number is taken from Android's own definition.
 */
#define UID AID_RADIO


util_void_t util_continue_as_non_privileged_user()
{
    UTIL_LOG_D("changing user to uid=%d", UID);
    setuid(UID); /* it is not possible to revert this operation. */
}


/* This function will change the user to radio but keep the needed
 * capability. It should be used only if needed.
 */
util_void_t util_continue_as_privileged_user(util_security_cap_t util_cap)
{
    UTIL_LOG_D("changing user to uid=%d", UID);

    prctl(PR_SET_KEEPCAPS, 1, 0, 0, 0);
    setuid(UID);

    struct __user_cap_header_struct header;
    struct __user_cap_data_struct cap;
    header.version = _LINUX_CAPABILITY_VERSION;
    header.pid = 0;

    cap.effective = cap.inheritable = 0;

    if (util_cap & UTIL_SECURITY_CAP_NET_ADMIN) {
      UTIL_LOG_D("keeping CAP_NET_ADMIN", UID);
      cap.effective |= 1 << CAP_NET_ADMIN;
    }

    if (util_cap & UTIL_SECURITY_CAP_SYS_ADMIN) {
      UTIL_LOG_D("keeping CAP_SYS_ADMIN", UID);
      cap.effective |= 1 << CAP_SYS_ADMIN;
    }

    if (!cap.effective) {
      UTIL_LOG_E("no capabilities to keep!");
      return;
    }

    cap.permitted = cap.effective;
    capset(&header, &cap);
}

#else /* end of ANDROID_BUILD */

util_void_t util_continue_as_non_privileged_user()
{
    UTIL_LOG_D("module test environment, not changing user.");
}


util_void_t util_continue_as_privileged_user(util_security_cap_t util_cap)
{
    (void)util_cap;
    UTIL_LOG_D("module test environment, not changing user.");
}


#endif /* non Android build */
