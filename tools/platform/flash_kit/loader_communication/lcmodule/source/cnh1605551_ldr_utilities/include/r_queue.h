/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef _R_QUEUE_H_
#define _R_QUEUE_H_

/**
 * @addtogroup ldr_utilities
 * @{
 *   @addtogroup queue
 *   @{
 *     Implementation of FIFO queue.
 *     Functions which names begin with Do_Fifo_* are non-reentrant.
 *     Functions which names begin with Do_RFifo_* are reentrant.
 *     Functions that are not interrupt safe are faster than the interrupt safe
 *     functions.
 *
 * @remark None of the functions check the sanity of the input parameters.
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include  "t_queue.h"
#include  "error_codes.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/

/**
 * @brief Creates a queue.
 *
 * @param [in]  Object_p         Pointer to LCM instance context.
 * @param [out] Queue_pp         After execution points to the allocated space
 *                               for the queue.
 * @param [in]  MaxLength        The maximum number of entries in the queue.
 * @param [in]  DestroyElement   Pointer to user defined function that
 *                               deallocates resources allocated for the
 *                               elements in the queue. NULL if there is no need
 *                               for deallocating resource.
 */
void Do_Fifo_Create(void *Object_p,
                    void **const Queue_pp,
                    const uint32 MaxLength,
                    void (*DestroyElement)(void *Element_p));

/**
 * @brief Releases any resources associated with the specified queue structure.
 *
 * @param [in]     Object_p      Pointer to LCM instance context.
 * @param [in,out] Queue_pp      Pointer to the queue structure to destroy.
 */
void Do_Fifo_Destroy(void *Object_p, void **const Queue_pp);

/**
 * @brief Enqueueing of pointers.
 *
 * @note Enqueueing and dequeueing are mutually reentrant, and all functions are
 *       reentrant across different queues, but enqueueing on the same queue is
 *       not necessarily reentrant.
 *
 * @param [in] Object_p         Pointer to LCM instance context.
 * @param [in] Queue_p          The queue to append to.
 * @param [in] Value_p          The value to enqueue.
 * @return     E_SUCCESS - The function completed successfully.
 *
 * @return     E_FAILED_TO_STORE_IN_FIFO - Failed to store data in fifo.
 */
ErrorCode_e Do_Fifo_Enqueue(void *Object_p,
                            void *const Queue_p,
                            void *const Value_p);

/**
 * @brief Dequeueing of pointers.
 *
 * Dequeueing function of a reentrant, interrupt-safe FIFO queue.
 *
 * @note Enqueueing and dequeueing are mutually reentrant, and all functions are
 *       reentrant across different queues, but dequeueing on the same queue is
 *       not necessarily reentrant.
 *
 * @param [in] Object_p    Pointer to LCM instance context.
 * @param [in] Queue_p     The queue to take an item from.
 *
 * @return                 The first pointer in the queue on success or NULL if
 *                         the queue is empty. Note that an enqueued NULL
 *                         pointer will be de-queued as a NULL pointer.
 */
void *Do_Fifo_Dequeue(void *Object_p, void *const Queue_p);

/**
 * @brief Registers an event listener for the specified queue.
 *
 * @note Only one listener per queue and event type is allowed.
 * @note A function invocation does not guarantee that the state of the queue is
 *       the same as the end-transition state, only that such a transition has
 *       happened.
 *
 * @param [in] Object_p  Pointer to LCM instance context.
 * @param [in] Queue_p   The queue for which to register a callback.
 * @param [in] Type      The type of event to register the callback for.
 *                       A value of EMPTY indicates that the specified callback
 *                       function should be called each time queue has
 *                       transitioned from a non-empty state to an empty state.
 *                       A value of NONEMPTY indicates that the callback func.
 *                       should be called each time the queue has transitioned
 *                       from an empty to a non-empty state.
 * @param [in] Callback  The function to call when the specified event occurs
 *                       or NULL to unregister a previously registered function.
 * @param [in] Param_p   Parameter to pass to the callback function.
 * @return               The previously registered callback function for this
 *                       type.
 * @return NULL          If no callback was previously registered.
 */
QueueCallback_fn Do_Fifo_SetCallback(void *Object_p, void *const Queue_p,
                                     const QueueCallbackType_e Type,
                                     const QueueCallback_fn Callback,
                                     void *const Param_p);

/**
 * @brief Determines the empty-status of the queue.
 *
 * @param [in] Object_p   Pointer to LCM instance context.
 * @param [in] Queue_p    The queue to inspect.
 * @return TRUE           If Fifo is empty or
 * @return FALSE          If Fifo is not empty.
 */
boolean Do_Fifo_IsEmpty(void *Object_p, const void *const Queue_p);

/**
 * @brief Checks if the provided element is member of the fifo.
 *
 * @param [in] Object_p   Pointer to LCM instance context.
 * @param [in] Queue_p    The queue to search for element.
 * @param [in] Value_p    The element to be searched in the queue.
 * @param [in] Match      Function that checks if two elements match.
 * @retval TRUE           If the element is member of the fifo.
 * @retval FALSE          If the element is not member of the fifo.
 */
boolean Do_Fifo_IsMember(void *Object_p,
                         const void *const Queue_p,
                         void *Value_p,
                         boolean(*Match)(void *Value1_p, void *Value2_p));

/**
 * @brief Returns the number of elements in the queue.
 *
 * @param [in] Object_p   Pointer to LCM instance context.
 * @param [in] Queue_p    Pointer to the queue.
 * @return                Number of elements in the queue.
 */
int Do_Fifo_GetNrOfElements(void *Object_p, const void *const Queue_p);


/**
 * @brief Creates a queue.
 *
 * @param [in]  Object_p       Pointer to LCM instance context.
 * @param [out] Queue_pp       After execution points to the allocated
 *                             space for the queue.
 * @param [in]  MaxLength      The maximum number of entries in the queue.
 * @param [in]  DestroyElement Pointer to user defined function that deallocates
 *                             resources allocated for the elements in the
 *                             queue. NULL if there is no need for deallocating
 *                             resource.
 */
void Do_RFifo_Create(void *Object_p,
                     void **const Queue_pp,
                     const uint32 MaxLength,
                     void (*DestroyElement)(void *Element_p));

/**
 * @brief Releases any resources associated with the specified queue structure.
 *
 * @param [in]     Object_p  Pointer to LCM instance context.
 * @param [in,out] Queue_pp  Pointer to the queue structure to destroy.
 */
void Do_RFifo_Destroy(void *Object_p, void **const Queue_pp);

/**
 * @brief Enqueueing of pointers.
 *
 * Enqueueing function of a re-entrant, interrupt-safe FIFO queue.
 *
 * @param [in]    Object_p        Pointer to LCM instance context.
 * @param [in]    Queue_p         The queue to append to.
 * @param [in]    Value_p         The value to enqueue.
 * @return        E_SUCCESS  The function completed successfully.
 *
 * @return        E_FAILED_TO_STORE_IN_FIFO - Faliled to store data in fifo.
 */
ErrorCode_e Do_RFifo_Enqueue(void *Object_p,
                             void *const Queue_p,
                             void *const Value_p);

/**
 * @brief Dequeueing of pointers.
 *
 * Dequeueing function of a reentrant, interrupt-safe FIFO queue.
 *
 * @param [in] Object_p   Pointer to LCM instance context.
 * @param [in] Queue_p    The queue to take an item from.
 * @return                The first pointer in the queue on success or NULL if
 *                        the queue is empty. Note that an enqueued NULL-pointer
 *                        will be de - queued as a NULL pointer.
 */
void *Do_RFifo_Dequeue(void *Object_p, void *const Queue_p);

/**
 * @brief Registers an event listener for the specified queue.
 *
 * @note Only one listener per queue and event type is allowed.
 * @note A function invocation does not guarantee that the state of the queue is
 *       the same as the end-transition state, only that such a transition has
 *       happened.
 *
 * @param [in] Object_p   Pointer to LCM instance context.
 * @param [in] Queue_p    The queue for which to register a callback.
 * @param [in] Type       The type of event to register the callback for.
 *                        - A value of EMPTY indicates that the specified
 *                        callback function should be called each time queue has
 *                        transitioned from a non-empty state to an empty state.
 *                        - A value of NONEMPTY indicates that the callback
 *                        function should be called each time the queue has
 *                        transitioned from an empty to a non-empty state.
 * @param [in] Callback   The function to call when the specified event occurs
 *                        or NULL to unregister a previously registered functions.
 * @param [in] Param_p    Parameter to pass to the callback function.
 * @return                The previously registered callback function for this
 *                        type.
 * @return  NULL          If no callback was previously registered.
 */
QueueCallback_fn Do_RFifo_SetCallback(void *Object_p, void *const Queue_p,
                                      const QueueCallbackType_e Type,
                                      const QueueCallback_fn Callback,
                                      void *const Param_p);

/**
 * @brief Determines the empty-status of the queue.
 *
 * @param [in] Object_p   Pointer to LCM instance context.
 * @param [in] Queue_p    The queue to inspect.
 * @retval  TRUE          If Fifo is empty or FALSE if is not empty.
 */
boolean Do_RFifo_IsEmpty(void *Object_p, const void *const Queue_p);

/**
 * @brief Checks if the provided element is member of the fifo.
 *
 * @param [in] Object_p   Pointer to LCM instance context.
 * @param [in] Queue_p    The queue to search for element.
 * @param [in] Value_p    The element to be searched in the queue.
 * @param [in] Match      Function that checks if two elements match.
 * @retval TRUE           If the element is member of the fifo.
 * @retval FALSE          If the element is not member of the fifo.
 */
boolean Do_RFifo_IsMember(void *Object_p,
                          const void *const Queue_p,
                          void *Value_p,
                          boolean(*Match)(void *Value1_p, void *Value2_p));

/**
 * @brief Returns the number of elements in the queue.
 *
 * @param [in] Object_p   Pointer to LCM instance context.
 * @param [in] Queue_p    Pointer to the queue.
 * @return                Number of elements in the queue.
 */
int Do_RFifo_GetNrOfElements(void *Object_p, const void *const Queue_p);

/** @} */
/** @} */
#endif /*_R_QUEUE_H_*/
