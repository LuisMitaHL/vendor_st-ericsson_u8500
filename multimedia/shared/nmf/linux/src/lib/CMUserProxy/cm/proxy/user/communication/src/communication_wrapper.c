/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#include <cm/os/api/cm_os.h>

#include <cm/proxy/api/private/stub-requiredapi.h>

PUBLIC EXPORT_SHARED t_cm_error CM_INTERNAL_PushEventWithSize(t_cm_bf_host2mpc_handle host2mpcId, t_event_params_handle h, t_uint32 size, t_uint32 methodIndex) {
   return CM_OS_PushEventWithSize(host2mpcId, h, size, methodIndex);
}

