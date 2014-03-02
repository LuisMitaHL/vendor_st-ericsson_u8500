#ifndef INCLUSION_GUARD_R_SERVICEREGISTER_H
#define INCLUSION_GUARD_R_SERVICEREGISTER_H
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/***************************************************************************
*
* DESCRIPTION:
*  Interface towards the service register. Available in all load modules,
*  allowing service function entries (req_funcs, do_funcs or flibs) to
*  find their server. Server could be located on the same CPU or connected
*  to through a linkhandler.
*
*************************************************************************/

/*************************************************************************
* Includes
*************************************************************************/
#include "t_basicdefinitions.h"
#include "t_serviceregister.h"

#ifdef __cplusplus
extern "C" {
#endif

/*************************************************************************
* Declaration of functions
**************************************************************************/

/**
 * Look up a signal based service globally, no attach.
 *
 * @param [in] Name_p           The name of the service.
 * @param [out] Pid_p           Will be filled in with the Pid for a registered
 *                              signal based service. Will be set to
 *                              \c NO_PROCESS if no match.
 *
 * @return Status for the request. Will return immediatly if *Pid_p != \c NO_PROCESS.
 *
 * @sigbased Yes
 * @signalid REQ_SR_RESOLVE
 * @waitmode Wait mode
 * @related None
 */
//************************************************************************
    SR_Result_t Do_SR_Resolve_Pid(
    const char *const Name_p,
    PROCESS * Pid_p);


/* MFL specific */
    extern SR_Result_t mfl_resolve_process_id(
        );
#define Do_SR_Resolve_Pid(process_name, process_id) mfl_resolve_process_id((process_name), (process_id))

#ifdef __cplusplus
}
#endif
#endif                          // #ifndef INCLUSION_GUARD_R_SERVICEREGISTER_H
