/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/**************************************************************************
*
* DESCRIPTION:
*
* OS specific functions porting for Linux.
*
*************************************************************************/
#include <stdio.h>
#include "malloc.h"
#include <pthread.h>
#include "smsipc.h"             //OS functions
#include "r_smslinuxporting.h"  //OS functions

static const char _File_[] = "linuxporting.c:";

const SMS_RequestControlBlock_t SMS_REQUEST_CONTROL_BLOCK_WAIT_MODE = { SMS_WAIT_MODE, SMS_CLIENT_TAG_NOT_USED, -1 };
const SMS_RequestControlBlock_t SMS_REQUEST_CONTROL_BLOCK_NO_WAIT_MODE = { SMS_NO_WAIT_MODE, SMS_CLIENT_TAG_NOT_USED, -1 };

const SMS_RequestControlBlock_t *const DONT_WAIT = &SMS_REQUEST_CONTROL_BLOCK_NO_WAIT_MODE;
const SMS_RequestControlBlock_t *const WAIT_RESPONSE = &SMS_REQUEST_CONTROL_BLOCK_WAIT_MODE;


/*
 *
 *
 * Implementation of SwBP related functions
 *
 *
 */
SMS_ClientTagStatus_t Do_SMS_ClientTag_Get(
    const void *const SigStruct_p,
    SMS_ClientTag_t * const ClientTag_p)
{
    SMS_B_ASSERT_(SMS_NIL != SigStruct_p);
    SMS_B_ASSERT_(NULL != ClientTag_p && SMS_NIL != ClientTag_p);

    *ClientTag_p = ((SMS_SigselectWithClientTag_t *) SigStruct_p)->ClientTag;

    return SMS_CLIENT_TAG_OK;
}

SMS_ClientTagStatus_t Do_SMS_ClientTag_Set(
    void *const SigStruct_p,
    const SMS_ClientTag_t ClientTagNew)
{
    SMS_B_ASSERT_(SMS_NIL != SigStruct_p);

    ((SMS_SigselectWithClientTag_t *) SigStruct_p)->ClientTag = ClientTagNew;

    return SMS_CLIENT_TAG_OK;
}

SMS_ClientTagStatus_t Do_SMS_ClientTag_Copy(
    void *const DestSignal_p,
    const void *const SourceSignal_p)
{
    SMS_B_ASSERT_(SMS_NIL != DestSignal_p && SMS_NIL != SourceSignal_p);

    ((SMS_SigselectWithClientTag_t *) DestSignal_p)->ClientTag = ((SMS_SigselectWithClientTag_t *) SourceSignal_p)->ClientTag;

    return SMS_CLIENT_TAG_OK;
}


void Do_SMS_RequestControlBlock_Init(
    SMS_RequestControlBlock_t * const ReqCtrlBlk_p)
{
    SMS_B_ASSERT_(NULL != ReqCtrlBlk_p && SMS_NIL != ReqCtrlBlk_p);

    ReqCtrlBlk_p->WaitMode = SMS_WAIT_MODE;
    ReqCtrlBlk_p->ClientTag = SMS_CLIENT_TAG_NOT_USED;
    ReqCtrlBlk_p->Socket = -1;
}

#ifdef CURRENTLY_UNUSED_CODE
void p_assert_print(
    const char *x,
    unsigned int y)
{
    SMS_IDENTIFIER_NOT_USED(x);
    SMS_IDENTIFIER_NOT_USED(y);
}
#endif

#ifdef PLATFORM_ANDROID
// Android does not seem to have a working assert() function.
// Therefore define something ourselves to print to the log!
void sms_assert_function(
    const uint8_t assert_result,
    const uint32_t line,
    const char *file_p,
    const char *assert_string_p)
{
    if (!assert_result) {
        SMS_LOG_E("%s line %d: ***!!!*** ASSERT FAILED ***!!!***", file_p, line);
        SMS_LOG_E("Failing conditional is: %s", assert_string_p != NULL ? assert_string_p : "<No Assert Text>");
    }
}
#endif


/*
 *
 *
 * Implementation of ported OSE-like signal related functions
 *
 *
 */
union SMS_SIGNAL *ose_to_ipc_sig_alloc(
    SMS_OSBUFSIZE size,
    SMS_SIGSELECT signo)
{
    //SMS_LOG_D("(alloc) called");
    union SMS_SIGNAL *sig = (union SMS_SIGNAL *) ipc_sig_alloc((unsigned int) size);
    sig->Primitive = signo;
    //SMS_LOG_D("(alloc) is done, returning %08X, set Primitive to %u, size to %u", (unsigned int)sig, (unsigned int)signo, (unsigned int)size);
    return sig;
}

/*
 * Start-up sequence handling functions.
 *
 * Use as follows:
 * 1. From thread 1, call start_up_sequence_lock().
 * 2. Start thread 2.
 * 3. From thread 1, call start_up_sequence_wait(). This call does not return
 *    before start_up_sequence_complete() has been called once.
 * 4. From thread 2, call start_up_sequence_complete() when startup is complete.
 * 5. Call by thread 1 to start_up_sequence_wait() finally returns.
 * 6. Repeat steps 1 through 5 for each startup sequencing that is needed.
 *    Or call start_up_sequence_destroy() when no more startup sequencing is required.
 *
 * Note: Steps 3 and 4 may occur in reverse order depending on how the threads are
 * scheduled. Nevertheless the call to start_up_sequence_wait() will not return
 * before a call to start_up_sequence_complete() is or has been made.
 */

static pthread_mutex_t start_up_sequence_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t start_up_sequence_cond = PTHREAD_COND_INITIALIZER;
static int start_up_sequence_flag = FALSE;

void start_up_sequence_lock()
{
    SMS_C_(SMS_LOG_V("entry"));
    // Lock mutex
    if (0 != pthread_mutex_lock(&start_up_sequence_mutex)) {
        SMS_B_(SMS_LOG_D("pthread_mutex_lock() failed"));
        exit(1);
    }

    // Re-set the flag
    start_up_sequence_flag = FALSE;

    SMS_C_(SMS_LOG_V("exit"));
}

void start_up_sequence_wait()
{
    SMS_C_(SMS_LOG_V("entry"));
    // Wait for condition to become signalled, and flag set
    do {
        if (0 != pthread_cond_wait(&start_up_sequence_cond, &start_up_sequence_mutex)) {
            SMS_A_(SMS_LOG_E("pthread_cond_wait() failed"));
            exit(1);
        }
    } while (TRUE != start_up_sequence_flag);

    // Unlock after wait. New lock must be taken before calling this function again
    if (0 != pthread_mutex_unlock(&start_up_sequence_mutex)) {
        SMS_A_(SMS_LOG_E("pthread_mutex_unlock() failed"));
        exit(1);
    }
    SMS_C_(SMS_LOG_V("exit"));
}

void start_up_sequence_complete()
{
    SMS_C_(SMS_LOG_V("entry"));
    // Must lock mutex before signalling condition, and setting flag
    if (0 != pthread_mutex_lock(&start_up_sequence_mutex)) {
        SMS_A_(SMS_LOG_E("pthread_mutex_lock() failed"));
        exit(1);
    }

    // Set the signalling flag
    start_up_sequence_flag = TRUE;

    if (0 != pthread_cond_signal(&start_up_sequence_cond)) {
        SMS_A_(SMS_LOG_E("pthread_cond_signal() failed"));
        exit(1);
    }

    // Unlock after signalling condition
    if (0 != pthread_mutex_unlock(&start_up_sequence_mutex)) {
        SMS_A_(SMS_LOG_E("pthread_mutex_unlock() failed"));
        exit(1);
    }
    SMS_C_(SMS_LOG_V("exit"));
}

void start_up_sequence_destroy()
{
    SMS_C_(SMS_LOG_V("entry"));
    // Release mutex and condition objects
    if ((0 != pthread_mutex_destroy(&start_up_sequence_mutex)) &&
        (0 != pthread_cond_destroy(&start_up_sequence_cond))) {
        SMS_A_(SMS_LOG_E("pthread_mutex_destroy() or pthread_cond_destroy() failed"));
    }
    SMS_C_(SMS_LOG_V("exit"));
}
