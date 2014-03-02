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
*
*/

#ifndef _R_SMSLINUXPORTING_H
#define _R_SMSLINUXPORTING_H

#include "t_smslinuxporting.h"
#include "t_sms.h"
#include "malloc.h"
#include "smsipc.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <assert.h>
#include <dirent.h>
#include <signal.h>
#include <pthread.h>
#include "util_log.h"


#ifdef PLATFORM_ANDROID
#ifndef LOG_TAG
#define LOG_TAG "SMS"
#endif
#include <utils/Log.h>
#else
#ifndef LOG_TAG
#define LOG_TAG "SMS"
#endif
#endif


/********************
* General macro's
*********************/
/**
 * Returns the smallest value of x and y.
 *
 * @param [in] x Value of any scalar type
 * @param [in] y Value of any scalar type
 *
 * @return The return type obeys the rules regarding binary conversion of the
 *         programming language C.
 *
 * @sigbased No - Macro, parameters expand more than one time
 */
#ifndef SMS_MIN
#define SMS_MIN(val1, val2) (((val1) < (val2)) ? (val1) : (val2))
#endif

/**
 * Returns the largest value of x and y.
 *
 * @param [in] x Value of any scalar type
 * @param [in] y Value of any scalar type
 *
 * @return The return type obeys the rules regarding binary conversion of the
 *         programming language C.
 *
 * @sigbased No - Macro, parameters expand more than one time
 */
#ifndef SMS_MAX
#define SMS_MAX(val1, val2) (((val1) > (val2)) ? (val1) : (val2))
#endif


//The following compile time assert macro can be used among statements
#define SMS_COMPILE_TIME_ASSERT_STATEMENT(Condition) (void)(1/(int)!!(Condition))


//The following compile time assert macro can be used among declarations

/*lint -esym(752, CompileTimeAssertTestVariable) inhibit lint-message about not used symbol */
/*lint -esym(762, CompileTimeAssertTestVariable) inhibit lint-message about redundantly declared symbol */

#define SMS_COMPILE_TIME_ASSERT_DECL(Condition) extern int CompileTimeAssertTestVariable[1/(int)!!(Condition)]


/**
 * Should be used to "touch" a variable or a formal parameter that is intentionally
 * not used and thereby avoid compiler warnings.
 *
 * @param [in] Id A symbol that represents a variable that is not used, usually a
 *                formal parameter.
 *
 * @sigbased No - Macro
 */
#ifdef SMS_IDENTIFIER_NOT_USED
#undef SMS_IDENTIFIER_NOT_USED
#endif
#define SMS_IDENTIFIER_NOT_USED(P) (void)(P);


/*
 *
 *
 *  The Sys Category
 *
 *
 */

/**
 * Return type for all the client tag help functions. See chapter 4 for more
 * details.
 *
 * @param SMS_CLIENT_TAG_OK Client Tag value is OK.
 * @param SMS_CLIENT_TAG_FAILED_UNPACKING An error ocurred while unpacking.
 * @param SMS_CLIENT_TAG_FAILED_PACKING An error ocurred while packing.
 * @param SMS_CLIENT_TAG_NOT_SUPPORTED Client Tag is not supported.
 */
typedef enum {
    SMS_CLIENT_TAG_OK,
    SMS_CLIENT_TAG_FAILED_UNPACKING,
    SMS_CLIENT_TAG_FAILED_PACKING,
    SMS_CLIENT_TAG_NOT_SUPPORTED
} SMS_ClientTagStatus_t;


/*************************************************************************
* Declaration of functions
**************************************************************************/

/**
 * This function returns the Client Tag information from the signal pointed to
 * by SigStruct_p. It may be used to obtain the client tag information after
 * having received an Event Channel signal or a response signal (response to a
 * request function with client tag support).
 *
 * @param [in] SigStruct_p Signal to be unpacked.
 * @param [out] Tag_p      Client tag information
 *
 * @retval GS_CLIENT_TAG_OK
 */
SMS_ClientTagStatus_t Do_SMS_ClientTag_Get(
    const void *const SigStruct_p,
    SMS_ClientTag_t * const ClientTag_p);


/**
 * This function sets the Client Tag information in the signal pointed to by
 * SigStruct_p to the value specified in ThisTag. It assumes that the signal
 * data structure has been defined using SigselectWithClientTag_t instead of
 * SIGSELECT.
 *
 * @param [in] SigStruct_p  Signal to be unpacked.
 * @param [in] ClientTagNew Client Tag value to copy into the signal.
 *
 * @retval GS_CLIENT_TAG_OK
 */
SMS_ClientTagStatus_t Do_SMS_ClientTag_Set(
    void *const SigStruct_p,
    const SMS_ClientTag_t ClientTagNew);


/**
 * This function copies the Client Tag information from the signal pointed to
 * by SourceSignal_p into the signal pointed to by Destination_p. It assumes
 * that the signal data structure of both signals has been defined using
 * SigselectWithClientTag_t instead of SIGSELECT.
 *
 * @param [in, out] DestSignal_p Pointer to the signal where the Client Tag will
 *                               be copied to (input), or has been copied to (output).
 * @param [in] SourceSignal_p    Pointer to the signal from where the Client Tag
 *                               will be copied. Destination_p Pointer to the
 *                               signal where the Client Tag will be copied to.
 * @retval GS_CLIENT_TAG_OK
 */
SMS_ClientTagStatus_t Do_SMS_ClientTag_Copy(
    void *const DestSignal_p,
    const void *const SourceSignal_p);



/**
 * This function initialises all the elements in ReqCtrlBlk_p. Then, the caller
 * is expected to set the elements that he uses.
 *
 * The reason is that the control block may contain other (hidden) elements
 * that must obtain a default value.
 *
 * @param [in, out] ReqCtrlBlk_p  Input: Pointer to the request control block that will be
 *                                       initialized.
 *                                Output: Pointer to the initialized block.
 * @return void
 */
void Do_SMS_RequestControlBlock_Init(
    SMS_RequestControlBlock_t * const ReqCtrlBlk_p);


#if 0
/**
 * Signal Info initialization function. Provides information about the signal.
 *
 * @param [in, out] SignalInfo_p Input:   SignalInfoBlock to intialize.
 *                               Output:  Intialized SignalInfo.
 * @param [in] Signal_p          The signal.
 *
 * @return void
 */
void Do_SwBP_SignalInfo_Init(
    SwBP_SignalInfo_t * const SignalInfo_p,
    const void *const Signal_p);

/**
 * Returns the Client tag from the signal information.
 *
 * @param [in] SignalInfo_p The signal information to get ClientTag from.
 *
 * @return ClientTag_t The ClientTag value.
 */
ClientTag_t Do_SwBP_SignalInfo_GetClientTag(
    const SwBP_SignalInfo_t * const SignalInfo_p);

/**
 * Returns the Process Id from the signal information
 *
 * @param [in] SignalInfo_p The SignalInfo to retrieve PROCESS id from.
 *
 * @return PROCESS The process id.
 */
PROCESS Do_SwBP_SignalInfo_GetProcessId(
    const SwBP_SignalInfo_t * const SignalInfo_p);
#endif


/*
 *
 *
 *  OS related functions
 *
 *
 */


#undef SMS_SEND
#undef SMS_RECEIVE
#undef SMS_SENDER
//Process <=> sender socket which will be stored in the signal struct to be fetched later by SENDER
#define SMS_SEND(Signal_pp, Process) ipc_send_signal_over_socket((int)Process, (void **)Signal_pp);
//Will extract the sender socket (instead of old OSE process ID)
#define SMS_SENDER( Signal_pp ) (SMS_PROCESS)ipc_get_request_fd((void*)(*Signal_pp))
#define SMS_EVENT_RECEIVER( Signal_p ) ipc_get_event_fd((void*)(Signal_p))
//Currently we have no better way than to actually send in the named variable "ReqCtrl_p->Socket".
//This function is defined in mmprot_os_linux.c
#define SMS_RECEIVE(sig_no) (union SMS_SIGNAL*)ose_to_ipc_sig_receive(sig_no, RequestCtrl_p->Socket)


/*
 *
 *
 *  OSE --> IPC porting "utility" functions, implemented in linuxporting.c
 *
 *
 */
union SMS_SIGNAL *ose_to_ipc_sig_alloc(
    SMS_OSBUFSIZE size,
    SMS_SIGSELECT signo);


#undef SMS_SIGNAL_ALLOC
#define SMS_SIGNAL_ALLOC(Type, Primitive)         ((Type*)ose_to_ipc_sig_alloc(sizeof(Type), Primitive))

#undef SMS_SIGNAL_UNTYPED_ALLOC
#define SMS_SIGNAL_UNTYPED_ALLOC(Size, Primitive) ((void*)ose_to_ipc_sig_alloc(Size, Primitive))



/**************************************
 *
 *  Heap handling functions
 *
 *  Macros for dynamic memory allocation and deallocation
 *
 */
#ifndef SMS_HEAP_CHECK
// Normal memory macros. No extra debugging enabled.
#define SMS_HEAP_ALLOC(t)     ((t*) malloc(sizeof(t)))
#define SMS_HEAP_UNTYPED_ALLOC(x)  malloc(x)
#define SMS_HEAP_REALLOC(x,y) realloc(x, y)
#define SMS_HEAP_FREE(Pointer_pp) do {free(*Pointer_pp); (*Pointer_pp) = NULL; } while (0)
#define SMS_HEAP_FREEP(Pointer_pp) SMS_HEAP_FREE(Pointer_pp)
#define SMS_HEAP_FREE_BUF(x) free(x)

#else

// Debug memory macros.  These can only be enabled when building the sms_server. They cannot be used
// with the test harness or libsms_server as they need to link to utility functions available to the
// sms_server build.
// The memory utilities and macros are defined in smutil.h.
#include "smutil.h"

#endif



/*
 * Mutex functions, for synchronizing thread start up sequence in a Linux daemon.
 *
 * A controller in the system typically
 * - locks the "start_up_sequence_mutex"
 * - spawns a thread,
 * - the spawned thread unlocks the mutex once it has started up completely
 * ... and the controller continues to spawn next thread.
 *
 * For example, in the IMS daemon, rtp and timer server are threads spawned from sip.c.
 * These threads unlock the mutex once they have started up.
 */

/*
 * Lock the start-up-sequence mutex.
 *
 */
void start_up_sequence_lock(void);

/*
 * Wait for start-up-sequence to complete.
 *
 */
void start_up_sequence_wait(void);

/*
 * Complete start-up-sequence.
 *
 */
void start_up_sequence_complete(void);

/*
 * Destroy the start-up-sequence mutex.
 *
 */
void start_up_sequence_destroy(void);

/*
 *
 *
 * Timer server functions
 *
 *
 * Legacy interface: Do_TimerServer_xxx
 *
 * Ported timer server implementation: timer_server_xxx
 */
#define Do_SMS_TimerServer_MilliSecondsSinceReset()    timer_server_ms_since_reset()
#define Do_SMS_TimerSet_1_MS_WCT(SigNo,Ct,To)          timer_server_set_ms(SigNo,Ct,To)
#define Do_SMS_TimerRecurrentSet_1_MS_WCT(SigNo,Ct,To) timer_server_recurrent_set_ms(SigNo,Ct,To)
#define Do_SMS_TimerReset_1_MS_WCT(SigNo,Ct)           timer_server_reset_ms(SigNo,Ct)

/*
 * Main entry function of the timer server thread.
 */
void *timer_server_thread(
    void *arg);

/*
 * Clean up and terminate and the timer server thread.
 */
void timer_server_kill_thread(
    void);

/*
 * Register a new user of the timer server.
 *
 * @param [in] timer_expired_fd  The file descriptor where the timer server shall write the "timer expired" message.
 *
 * When used with the IPC-functions, the user will typically do the following calls:
 * 1. p_server_context = ipc_server_context_create(...);
 * 2. timer_server_user_register( ipc_server_context_get_internal_pipe_write_fd( p_server_context ) );
 */
void timer_server_user_register(
    const int timer_expired_fd);

/*
 * Deregister a timer server user.
 *
 * Note: The user is identified by the calling thread.
 */
void timer_server_user_deregister(
    void);

/*
 * Fetch number of milliseconds since reset.
 *
 * @return uint32 Number of ms since reset.
 */
uint32_t timer_server_ms_since_reset(
    void);

/*
 * Start a timer.
 *
 * @param [in] sig_no      The timer type, which is a signal ID, e.g. REFRESH_TIMER.
 * @param [in] client_tag  The timer ID. Several timers of the same type can be started.
 * @param [in] period_ms   The timer period in ms.
 */
void timer_server_set_ms(
    const uint32_t sig_no,
    const uint32_t client_tag,
    const uint32_t period_ms);

/*
 * Start a recurrent timer.
 *
 * @param [in] sig_no      The timer type, which is a signal ID, e.g. REFRESH_TIMER.
 * @param [in] client_tag  The timer ID. Several timers of the same type can be started.
 * @param [in] period_ms   The timer period in ms.
 */
void timer_server_recurrent_set_ms(
    const uint32_t sig_no,
    const uint32_t client_tag,
    const uint32_t timer_period);

/*
 * Reset a timer.
 *
 * @param [in] sig_no      The timer type, which is a signal ID, e.g. REFRESH_TIMER.
 * @param [in] client_tag  The timer ID. Several timers of the same type can be started.
 */
void timer_server_reset_ms(
    const uint32_t sig_no,
    const uint32_t client_tag);



 /**************************************
 *
 *
 *  Debug Macros
 *
 *
 **************************************/

#ifdef PLATFORM_ANDROID
// Android does not seem to have a working assert() function.
// Therefore define something ourselves to print to the log!
void sms_assert_function(
    const uint8_t assert_result,
    const uint32_t line,
    const char *file_p,
    const char *assert_string_p);

#define SMS_ASSERT_FUNC(x) sms_assert_function((uint8_t)(x), __LINE__, __FILE__, #x)
#else
#define SMS_ASSERT_FUNC(x) assert(x)
#endif

#ifdef PLATFORM_ANDROID
#define SMS_LOG_D(...)  util_print_log(UTIL_LOG_MODULE_SMS, sms_get_log_level(), UTIL_LOG_TYPE_DEBUG, __func__, __VA_ARGS__)
#define SMS_LOG_E(...)  util_print_log(UTIL_LOG_MODULE_SMS, sms_get_log_level(), UTIL_LOG_TYPE_ERROR, __func__, __VA_ARGS__)
#define SMS_LOG_I(...)  util_print_log(UTIL_LOG_MODULE_SMS, sms_get_log_level(), UTIL_LOG_TYPE_INFO, __func__, __VA_ARGS__)
#define SMS_LOG_V(...)  util_print_log(UTIL_LOG_MODULE_SMS, sms_get_log_level(), UTIL_LOG_TYPE_VERBOSE, __func__, __VA_ARGS__)
#define SMS_LOG_W(...)  util_print_log(UTIL_LOG_MODULE_SMS, sms_get_log_level(), UTIL_LOG_TYPE_WARNING, __func__, __VA_ARGS__)
#else
#define SMS_LOG_E(format, ...)
#define SMS_LOG_W(format, ...)
#define SMS_LOG_I(format, ...)
#define SMS_LOG_D(format, ...)
#define SMS_LOG_V(format, ...)
#endif

#ifdef SMS_PRINT_A_
#define SMS_A_(x) x
#define SMS_A_ASSERT_(x) SMS_ASSERT_FUNC(x)
#else
#define SMS_A_(x)
#define SMS_A_ASSERT_(x)
#endif
#ifdef SMS_PRINT_B_
#define SMS_B_(x) x
#define SMS_B_ASSERT_(x) SMS_ASSERT_FUNC(x)
#else
#define SMS_B_(x)
#define SMS_B_ASSERT_(x)
#endif
#ifdef SMS_PRINT_C_
#define SMS_C_(x) x
#define SMS_C_ASSERT_(x) SMS_ASSERT_FUNC(x)
#else
#define SMS_C_(x)
#define SMS_C_ASSERT_(x)
#endif

int sms_log_select_callback(const int fd, const void *data_p);
void sms_set_log_level(util_log_type_t log_level);
util_log_type_t sms_get_log_level();
int sms_log_init();
void sms_log_close();

#endif /* _R_SMSLINUXPORTING_H */
