/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* DESCRIPTION:
*
*     Portable types used for a consistent target platform.
*     The name should make it totally clear as to what they are used for.
*
*/

#ifndef _T_SMSLINUXPORTING_H
#define _T_SMSLINUXPORTING_H

#include <stdint.h>
#include <limits.h>
#include "t_sms_cb_session.h"

/*******************
* MAX and MIN values
********************/
/** Maximum value for a entity of type uint8 */
#define SMS_MAX_UINT8  (UCHAR_MAX)
/** Maximum value for a entity of type uint32 */
#define SMS_MAX_UINT32 (ULONG_MAX)


/*
 *
 *
 *
 *
 *  OS related types, for compiling code that assumes OSE types.
 *
 *
 *
 *
 */

typedef int SMS_PROCESS;        // means file descriptor
typedef uint32_t SMS_OSBUFSIZE;
#define SMS_NO_PROCESS ((SMS_PROCESS)-1)
#define SMS_NIL (0)



/**
 * Used for Events with Confirm in the classical SwBP style.
 *
 * @param ProcessId The process id
 * @param ClientTag The Client tag value.
 */
typedef struct {
    SMS_PROCESS ProcessId;
    SMS_ClientTag_t ClientTag;
} SMS_SwBP_SignalInfo_t;


/**
* Holds the version for a category.
*/
//typedef uint16_t GS_CategoryVersion_t;

extern const SMS_RequestControlBlock_t SMS_REQUEST_CONTROL_BLOCK_WAIT_MODE;
extern const SMS_RequestControlBlock_t SMS_REQUEST_CONTROL_BLOCK_NO_WAIT_MODE;

/**
 * This is a constant pointer to a SMS_RequestControlBlock_t constant. Its request
 * mode is set to SMS_NO_WAIT_MODE, its ClientTag is set to SMS_CLIENT_TAG_NOT_USED and
 * ClientHandlers_p is set to NULL. This is used in the normal case when the
 * client is satisfied with default values in the control block.
 *
 * Example:
 * Request_<Object>_<Operation>(SMS_REQUEST_CONTROL_BLOCK_NO_WAIT_MODE_p,
 *                              AnyClientInParmeter,
 *                              AnyClientOutParameter_p);
 */

extern const struct SMS_RequestControlBlock_t *const SMS_REQUEST_CONTROL_BLOCK_NO_WAIT_MODE_p;

/**
 * This is constant pointer to a SMS_RequestControlBlock_t constant. Its request
 * mode is set to SMS_WAIT_MODE, its ClientTag is set to SMS_CLIENT_TAG_NOT_USED and
 * ClientHandlers_p is set to NULL. This is used in the normal case when the
 * client are satisfied with default values in the control block.
 *
 * Example:
 * Request_<Object>_<Operation>(SMS_REQUEST_CONTROL_BLOCK_WAIT_MODE_p,
 *                              AnyClientInParmeter,
 *                              AnyClientOutParameter_p);
 */
extern const struct SMS_RequestControlBlock_t *const SMS_REQUEST_CONTROL_BLOCK_WAIT_MODE_p;


#endif                          //_T_SMSLINUXPORTING_H
