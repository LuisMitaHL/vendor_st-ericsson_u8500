/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#include <cm/inc/cm_def.h>

PUBLIC EXPORT_SHARED void CM_GetVersion(t_uint32 *version) {
    *version = NMF_VERSION;
}
