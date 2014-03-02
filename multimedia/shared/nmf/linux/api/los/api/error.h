/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#ifndef __INC_CM_SERVICES_ERROR_H
#define __INC_CM_SERVICES_ERROR_H

#include <cm/inc/cm_type.h>
#include <los/api/thread.h>
#include <los/api/log.h>

#define CHECK_NMF_ERROR(myMsg,myExpr) \
{ \
    t_nmf_error myError; \
    if ( \
            (myError = (myExpr)) != CM_OK \
       ) \
    { \
        LOS_Log("Error: %s (error = %s (%d))\n", (void*)(myMsg), (void*)CM_StringError(myError), (void*)(int)myError, 0x0, 0x0, 0x0); \
        LOS_Exit(); \
    } \
}

#define CHECK_CM_ERROR(myMsg,myExpr) CHECK_NMF_ERROR(myMsg,myExpr)

#endif /* __INC_CM_SERVICES_ERROR_H */
