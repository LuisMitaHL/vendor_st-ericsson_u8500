/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Public header file for the PWL Pulse width light modulator module
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef _HCL_PWL_H_
#define _HCL_PWL_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/
#ifndef _HCL_DEFS_H
#include "hcl_defs.h"
#endif
#ifndef __INC_DBG_H
#include "debug.h"
#endif


/* Version identifiers */
#define PWL_HCL_VERSION_ID  2
#define PWL_HCL_MAJOR_ID    0
#define PWL_HCL_MINOR_ID    8

typedef enum
{
    PWL_NO_PENDING_EVENT_ERROR          = HCL_NO_PENDING_EVENT_ERROR,
    PWL_NO_MORE_FILTER_PENDING_EVENT    = HCL_NO_MORE_FILTER_PENDING_EVENT,
    PWL_NO_MORE_PENDING_EVENT           = HCL_NO_MORE_PENDING_EVENT,
    PWL_REMAINING_FILTER_PENDING_EVENTS = HCL_REMAINING_FILTER_PENDING_EVENTS,
    PWL_REMAINING_PENDING_EVENTS        = HCL_REMAINING_PENDING_EVENTS,
    PWL_INTERNAL_EVENT                  = HCL_INTERNAL_EVENT,
    PWL_OK                              = HCL_OK,
    PWL_INVALID_PARAMETER               = HCL_INVALID_PARAMETER,
    PWL_UNSUPPORTED_HW                  = HCL_UNSUPPORTED_HW,
    PWL_UNSUPPORTED_FEATURE             = HCL_UNSUPPORTED_FEATURE
} t_pwl_error;

/*Integration Test Mode 
--------------------------------------*/
typedef enum
{
     PWL_INTEGRATION_TEST_DISABLE,
     PWL_INTEGRATION_TEST_ENABLE
} t_pwl_integ_test_mode;

typedef enum {
     PWL_DRIVE_0,
     PWL_DRIVE_1
} t_pwl_drive_level;


/*Initialisation function*/
t_pwl_error PWL_Init(t_logical_address pwl_base_address);

/*PWL Functions*/
t_pwl_error PWL_SetLevel(t_uint32 pwl_level);
t_pwl_error PWL_GetLevel(t_uint32 *pwl_level);
t_pwl_error PWL_Enable(void);
t_pwl_error PWL_Disable(void);

/*Version and Debug Functions*/
t_pwl_error PWL_GetVersion(t_version *p_version);
t_pwl_error PWL_SetDbgLevel(t_dbg_level debug_level);
t_pwl_error PWL_GetDbgLevel(t_dbg_level *p_debug_level);
t_pwl_error PWL_SetIntegrationTestMode(IN t_pwl_integ_test_mode pwl_integ_test_mode);
t_pwl_error PWL_ProgramIntegrationTestOutputs(IN t_pwl_drive_level pwl_drive_level);

/*PWL Reset function*/
t_pwl_error PWL_Reset(void);


#ifdef __cplusplus
}   /* allow C++ to use these headers */
#endif /* __cplusplus */
#endif /* #ifndef _HCL_PWL_H_ */

