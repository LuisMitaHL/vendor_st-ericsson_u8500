/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <dlfcn.h>

#define LOG_TAG "admsrv"
#include <cutils/log.h>

#include <sys/prctl.h>
#include <linux/capability.h>
#include <private/android_filesystem_config.h>

#define ADM_SO_FILE "/system/lib/libste_adm.so"

typedef int (*adm_create_func_t)(void);

static void setup_capabilities()
{
    struct __user_cap_header_struct header;
    struct __user_cap_data_struct data;
    struct __user_cap_data_struct data_backup;

    header.version = _LINUX_CAPABILITY_VERSION;
    header.pid = getpid();

    if (capget(&header, &data_backup)) {
        ALOGE("capget failed : %s\n", strerror(errno));
    }

    // Request not clear capabilities when dropping root
    // This has the following effect:
    //   A thread's effective capability set is always cleared when such a credential
    //   change is made, regardless of the setting of the "keep capabilities" flag.
    if (prctl(PR_SET_KEEPCAPS, 1, 0, 0, 0)) {
        ALOGE("PR_SET_KEEPCAPS failed for service : %s\n", strerror(errno));
    }

    // Request back capabilities we had
    data.effective = data_backup.effective;
    data.permitted = data_backup.permitted;
    data.inheritable = data_backup.inheritable;

    if (capset(&header, &data)) {
        ALOGE("capset backup failed : %s\n", strerror(errno));
    }

    // switch user to "audio"; this may fail if we are not root or "audio"
    if (setuid(AID_AUDIO)) {
        ALOGE("setuid failed : %s\n", strerror(errno));
    }

    if (data_backup.inheritable) {
        // Setup the expected final capabilities
        data.effective = data.permitted = data.inheritable = data_backup.inheritable;
        if (capset(&header, &data)) {
            ALOGE("capset failed : %s\n", strerror(errno));
        }
    }
}

int main(void)
{
    void *adm_handle_p;
    adm_create_func_t adm_create_func_p;

    setup_capabilities();

    adm_handle_p = dlopen(ADM_SO_FILE, RTLD_NOW);

    if (!adm_handle_p) {
        ALOGE("Failed to open %s - %s!\n", ADM_SO_FILE, dlerror());
        return 1;
    } else {
        adm_create_func_p = dlsym(adm_handle_p, "ste_adm_server_create");

        if (adm_create_func_p == NULL) {
            ALOGE("dlsym(ste_adm_server_create) failed: %s - "
                "Audio will not be available!\n", dlerror());
            dlclose(adm_handle_p);
            return 1;
        } else {
            (*adm_create_func_p)();
        }
    }

    // The ADM create function call will never return unless its threads are terminated so just return
    ALOGE("ADM server terminated\n");
    dlclose(adm_handle_p);
    return 0;
}
