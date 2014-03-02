/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/** \file cmproxy.h
 *
 * Simple header to define Linux specific things within the CM User Proxy
 */

#include <inc/typedef.h>
#include <inc/nmf_type.idt>
#include <nmf/inc/service_type.h>
#define LOG_TAG "CMUserProxy"
#include "log-utils.h"

#ifdef ANDROID
/** Android build system install everything in the RO /system partition
    Change requested for FIDO CR270847 */
#define REPOSITORY_DIR "/system/usr/share/nmf/repository/"
#else
/** For linux we keep it as it used to be */
#define REPOSITORY_DIR "/usr/share/nmf/repository/"
#endif

void cm_debugfs_init(void);
int cm_has_debugfs(void);
void cm_debugfs_dump(t_nmf_service_data *data);
int remove_directory(const char *) ;


