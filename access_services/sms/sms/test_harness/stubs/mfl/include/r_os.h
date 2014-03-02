/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* DESCRIPTION:
*
*  OS Abstraction functions prototypes and macros
*
******************************************************************************/

#ifndef INCLUSION_GUARD_R_OS_H
#define INCLUSION_GUARD_R_OS_H

#include <stdlib.h>
#include <stddef.h>
#include "t_basicdefinitions.h"

#include "t_os.h"

#ifndef REMOVE_MFL_SIGNAL_MACROS
#include "r_smslinuxporting.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


/******************************************************************************
*  SIGNAL ALLOCATION
******************************************************************************/


/**
 *  Allocate a signal from the signal pool. The signal size is sizeof(Type)
 *  The signal primitive it written first in the signal. The allocation never fails.
 *  If there are no more signals available this is treated as a fatal error
 *  and the error handler is called.
 *
 * @param [in] Type      Any type, Signal Structure type.
 * @param [in] Primitive SIGSELECT, Signal primitive (first uint32 in the signal)
 * @return Type*, Returns a pointer to the allocated signal buffer (Signal_p).
 *                The allocated signal buffer is cache aligned.
 *
 * @sigbased No
 * @waitmode Wait mode.
 * @related There are no related events.
 */
/* #define SIGNAL_ALLOC(Type, Primitive)         ((Type*)alloc(sizeof(Type), (Primitive))) */

/* MFL specific */
#ifdef REMOVE_MFL_SIGNAL_MACROS
    extern void *mfl_signal_alloc(
        );
#define SIGNAL_ALLOC(Type, SignalId) mfl_signal_alloc(sizeof(Type), (SignalId))
#else
//#define SMS_SIGNAL_ALLOC(Type, Primitive)         ((Type*)ose_to_ipc_sig_alloc(sizeof(Type), (Primitive)))
#define SIGNAL_ALLOC(Type, SignalId) SMS_SIGNAL_ALLOC(Type, SignalId)
#endif

/* Forse a crach */
#define RESTORE( Signal_p )  \
  do { \
    char *foo = NULL; *foo = 1; \
  } while (0);

/* Forse a crach */
#define SEND_SIMPLE_PRIMITIVE( Primitive, Process ) \
  do { \
    char *foo = NULL; *foo = 1; \
  } while (0);

/**
 *  Allocate a signal from the signal pool.
 *  The signal primitive it written first in the signal. The allocation never fails.
 *  If there are no more signals available this is treated as a fatal error
 *  and the error handler is called. The allocated signal buffer is cache aligned.
 *
 * @param [in] Size      OSBUFSIZE, The requested size of the signal in bytes.
 * @param [in] Primitive SIGSELECT, Signal primitive (first uint32 in the signal)
 * @return Returns union SIGNAL*, A pointer to the allocated signal buffer (Signal_p)
 *
 * @sigbased No
 * @waitmode Wait mode.
 * @related There are no related events.
 */
/* #define SIGNAL_UNTYPED_ALLOC(Size, Primitive) ((void*)alloc((Size),       (Primitive))) */

/* MFL specific */
#define SIGNAL_UNTYPED_ALLOC(Size, SignalId) mfl_signal_alloc((Size), (SignalId))

/* TODO remove this void* RSwBP_SignalAlloc(const size_t    Size, 
                        const SIGSELECT SignalId); */


/**
 * Releases a Signal. Only the process that owns the signal is allowed to perform this operation.
 *
 * @param [in,out] Signal_pp union SIGNAL**, The Signal pointer pointer to the signal that
 *                           should be freed, the signal pointer will be NIL'ed.
 *                           Signal_pp must point to a valid signal, and may not be NULL
 * @return void
 *
 * @sigbased No
 * @waitmode Wait mode.
 * @related There are no related events.
 */
/* #define SIGNAL_FREE(Signal_pp) free_buf((union SIGNAL**)(Signal_pp)) */
/* MFL specific */
#ifdef REMOVE_MFL_SIGNAL_MACROS
    extern void mfl_signal_free(
        );
#define SIGNAL_FREE(Signal_pp) mfl_signal_free(Signal_pp)
#else
//#define SMS_SIGNAL_FREE(Signal_pp) ipc_sig_free_buf((void**)(Signal_pp))
#define SIGNAL_FREE(Signal_pp) SMS_SIGNAL_FREE(Signal_pp)
#endif

/* TODO remove this void RSwBP_Free(void** Signal_pp); */



/******************************************************************************
*  SIGNAL SENDING
******************************************************************************/


/**
 *  Send a signal. After this function call the signal are no longer owned by the calling process.
 *
 * @param [in,out] Signal_pp union SIGNAL**, The Signal pointer pointer to the signal that
 *                           should be sent, the signal pointer will be NIL'ed.
 * @param [in] Process       PROCESS, The process ID to send the signal to.
 * @return void
 *
 * @sigbased No
 * @waitmode Wait mode.
 * @related There are no related events.
 */
    /*
       #define SEND( Signal_pp, Process ) \
       send( (union SIGNAL **)Signal_pp, Process )
     */
/* MFL specific */
    extern void mfl_signal_send(
    void **signal_ptr_ptr,
    PROCESS process_id);
#define SEND(Signal_pp, PId) mfl_signal_send((Signal_pp), (PId))

/******************************************************************************
* SIGNAL RECEIVING
******************************************************************************/


/**
 *  Wait for the signals in the Primitives list. This is an array where the first
 *  element contains the number of primitives, and the other elements
 *  contain the signal numbers that are expected.
 *
 *  Examples of primitive lists:
 *  { 2, SIGNAL_1, SIGNAL_2 } (will wait for SIGNAL_1 or SIGNAL_2)
 *  { 0 } (will wait for any signal)
 *
 *  If not called from an interrupt
 *  process the function will return when the selected signals are received and
 *  will always return with a signal.
 *  If called from interrupt process the function returns directly;
 *  if there is no signal it returns NIL.
 *
 * @param [in] Primitives const SIGSELECT*, Signal primitive list
 * @return void*, Signal (or NIL pointer if called from interrupt process)
 *
 * @sigbased No
 * @waitmode Wait mode.
 * @related There are no related events.
 */
/*
#define RECEIVE( Primitives ) \
  (void *)receive( Primitives )
*/
/* MFL specific */
#define RECEIVE(SignalSpec_p) NIL;      /* Shall not be called but will be needed to compile */

/******************************************************************************
*  OTHER SIGNAL MACROS
******************************************************************************/


/**
 * Get the sender of the signal.
 *
 * @param [in] Signal_pp union SIGNAL**, The Signal pointer pointer to the signal
 *                       that should be analyzed.
 * @return Sender PROCESS, Process ID
 *
 * @sigbased No
 * @waitmode Wait mode.
 * @related There are no related events.
 */

/*
#define SENDER( Signal_pp ) \
  sender( (union SIGNAL **)Signal_pp )
*/
/* MFL specific */

#ifdef REMOVE_MFL_SIGNAL_MACROS
    extern PROCESS mfl_signal_get_p_id(
        );
#define SENDER(Signal_pp) mfl_signal_get_p_id((Signal_pp))
#else
//#define SMS_SENDER( Signal_pp ) (SMS_PROCESS)ipc_get_request_fd((void*)(*Signal_pp))
#define SENDER(Signal_pp) SMS_SENDER(Signal_pp)
#endif

//------------------------------------------------------------------------------
// HEAP MACROS
//------------------------------------------------------------------------------

/**
 * Allocates a heap buffer that is big enough to fit a value of the given type.
 * The allocation is "safe", i.e. when the process that allocates the process dies,
 * the allocated block will be automatically freed.
 *
 * The buffer should be freed using HEAP_FREE or HEAP_UNSAFE_FREE. If HEAP_FREE is used, only the
 * process that allocated the buffer is allowed to free the buffer.
 *
 * @param [in] Type Any C type, A heap buffer that is sizeof the given type (in bytes)
 *
 * @return a pointer to the heap buffer that is casted to (Type*),
 *         or NULL if not enough memory was available.
 * @sigbased No
 */
#define HEAP_ALLOC(type) \
  malloc(sizeof(type))

/**
 * Allocates a heap buffer of the given size. The allocation is "safe", i.e. when the
 * process that allocates the process dies, the allocated buffer will be automatically freed.
 *
 * The buffer should be freed using HEAP_FREE or HEAP_UNSAFE_FREE. If HEAP_FREE is used, only the
 * process that allocated the buffer is allowed to free the buffer.
 *
 * @param [in] Size size_t, The size of the requested heap buffer in bytes
 *
 * @return void*, A pointer to the allocated buffer, or NULL if there was not enough memory available.
 * @sigbased No
 */
#define HEAP_UNTYPED_ALLOC(size)  malloc((size))

/**
 * Allocates a heap buffer of the given size. The allocation is "unsafe", i.e. when the
 * process that allocates the process dies, the allocated buffer will not be automatically freed.
 * Any process is allowed to free the buffer.
 *
 * The buffer should be freed using HEAP_FREE or HEAP_UNSAFE_FREE.
 *
 * @param [in] Size size_t, The size of the requested heap buffer in bytes
 *
 * @return void*, a pointer to the allocated buffer, or NULL if there was not enough memory available.
 * @sigbased No
 */
#define HEAP_UNSAFE_UNTYPED_ALLOC(size) malloc((size))

/**
 * Equal to HEAP_UNTYPED_ALLOC, allocates NumberOfElements*ElementSize bytes.
 * The only difference is that it also sets all bytes in the buffer to 0.
 *
 * The buffer should be freed using HEAP_FREE or HEAP_UNSAFE_FREE.
 *
 * @param [in] NumberOfElements size_t, The number of elements that should fit in the buffer
 * @param [in] ElementSize      size_t, The size of an individual element in bytes
 *
 * @return void*, a pointer to the allocated buffer, or NULL if there was not enough memory available.
 * @sigbased No
 */
#define HEAP_CLEAR_ALLOC(number_of_elements, element_size) calloc((number_of_elements), (element_size))

/**
 * Frees the given heap buffer. If the buffer was allocated in a "safe" way, it is checked that the
 * current process is the owner of the buffer.
 *
 * @param [in, out] Pointer_pp void**, Address of the pointer to the heap buffer that is to be freed.
 *                                     *Pointer_pp may be NULL.
 *                                     The pointer will be changed to NULL by this call.
 * @return void
 * @sigbased No
 */
#define HEAP_FREE(mem_pp) free((mem_pp))

/**
 * Frees the given heap buffer. No check is made that the
 * current process is the owner of the buffer, even if the buffer was allocated in a "safe" way.
 *
 * @param [in, out] Pointer_pp void**, Address of the pointer to the heap buffer that is to be freed.
 *                                     The pointer will be changed to NULL by this call.
 *                                     *Pointer_pp may be NULL.
 * @return void
 * @sigbased No
 */
#define HEAP_UNSAFE_FREE(pointer_pp) free((pointer_pp))



#ifdef __cplusplus
}
#endif
#endif                          /* INCLUSION_GUARD_R_OS_H */
