#ifndef INCLUSION_GUARD_T_SERVICEREGISTER_H
#define INCLUSION_GUARD_T_SERVICEREGISTER_H
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/**************************************************************************
*
* DESCRIPTION:
*  Interface towards the service register. Available in all load modules,
*  allowing service function entries (req_funcs, do_funcs or flibs) to
*  find their server. Server could be located on the same CPU or connected
*  to through a linkhandler.
*
*************************************************************************/

//*************************************************************************
/* Includes
*************************************************************************/
#include "t_os.h"
//Remote SwBP specific  #include "t_catdefinitions.h"

//*************************************************************************
/* Types, constants and external variables
*************************************************************************/

/*Remote SwBP specific  
#define CAT_ID_SERVICEREGISTER 339
CAT_INTERVAL_DEFINITION(CAT_ID_SERVICEREGISTER)
*/
#define SR_TIMEOUT_INFINITY (0xFFFFFFFF)
#define SR_MAX_NAME_LENGTH (63)

#ifdef __cplusplus
extern "C" {
#endif

/** \c SR_Options_t is used as a combined input type. Currently it handles
    visibility, local or global. */
    typedef enum SR_Options_t {
        SR_VISIBILITY_LOCAL,    /* register service only on local CPU */
        SR_VISIBILITY_GLOBAL,   /* register service globally on master CPU */
        SR_NOT_AVAILABLE,       /* indicate that service will never be available */
    } SR_Options_t;

/** \c SR_Result_t is the return type of the service register functions. */
    typedef enum SR_Result_t {
        SR_RESULT_GENERAL_ERROR = -1,
        SR_RESULT_OK = 0,
        SR_RESULT_E_BAD_PARAM,
        SR_RESULT_E_SERVICE_EXISTS,
        SR_RESULT_E_OUT_OF_MEMORY,
        SR_RESULT_E_NOT_FOUND,
        SR_RESULT_E_NOT_AVAILABLE,      /* service is registered as never available */
    } SR_Result_t;

/** \c CreateCallBack_t is the function pointer type used for the create callbacks. */
    typedef PROCESS(
    *SR_CreateCallBack_t) (
    void *Opaque_p,
    PROCESS Bid);

/** \c SR_RegisterEntry_t can be used by a programs starup code to register
    multiple interfaces at once. */
    typedef struct SR_RegisterEntry_t {
        char Name[SR_MAX_NAME_LENGTH + 1];
        SR_Options_t Options;
        PROCESS Pid;
        const void *Vtbl_p;
        SR_CreateCallBack_t CreateCallBack;
        void *Opaque_p;
        PROCESS Bid;
    } SR_RegisterEntry_t;

#ifdef __cplusplus
}
#endif
#endif                          /* #ifndef INCLUSION_GUARD_T_SERVICEREGISTER_H */
