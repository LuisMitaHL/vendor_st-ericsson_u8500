/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/**
 * \internal
 */
#ifndef REPOSITORY_H_
#define REPOSITORY_H_

#include <cm/inc/cm_type.h>
#include <cm/proxy/inc/osal.h>
#include <inc/nmf-limits.h>

t_cm_error cm_openComponent(const char *templateName, t_nmf_osal_fs_file_handle *file, void **data, t_cm_size *size, t_bool userspace);
void cm_closeComponent(t_nmf_osal_fs_file_handle file);

t_cm_error cm_pushComponent(int nb, char fileList[][MAX_INTERFACE_TYPE_NAME_LENGTH]);

t_cm_error cm_openComponents(
        int nb,
        char fileList[][MAX_INTERFACE_TYPE_NAME_LENGTH],
        t_nmf_osal_fs_file_handle files[],
        void *datas[],
        t_cm_size sizes[],
        t_bool userspace);
void cm_closeComponents(
        int nb,
        t_nmf_osal_fs_file_handle files[]);

#endif /*REPOSITORY_H_*/
