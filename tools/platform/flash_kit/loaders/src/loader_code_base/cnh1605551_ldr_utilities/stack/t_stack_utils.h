/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _T_STACK_H_
#define _T_STACK_H_

/**
 *  @addtogroup loader_utilities
 *  @{
 *    @addtogroup stack
 *    @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"

/**
 * @brief Stack type. Use with the Do_Stack_* functions. Example:
 * @code
 * void StackExample() {
 *   Stack_t Stack;
 *   void *p = Stack, *q;
 *   Do_Stack_Initialize(&Stack, 5);
 *   Do_Stack_Enqueue(&Stack, p);
 *   Do_Stack_Dequeue(&Stack);
 *   Do_Stack_Finalize(&Stack);
 * }
 * @endcode
 */
typedef struct Stack {
    int Head; /**< Head index of the buffer. This is where items are
                 inserted. */
    int Tail; /**< Tail index of the buffer. This is where items are
                 removed. */
    int Size; /**< Size of the buffer. */
    uint32 *Buffer_p; /**< Buffer pointer. */

    /** Empty buffer callback pointer. */
    void (*EmptyCallback)(struct Stack *Stack_p, void *Param_p);
    void *EmptyParam_p; /**< The parameter used for the empty buffer
                           transition callback. */

    /** NonEmpty buffer callback pointer. */
    void (*NonEmptyCallback)(struct Stack *Stack_p, void *Param_p);
    void *NonEmptyParam_p; /**< The parameter used for the non-empty buffer
                           transition callback. */
} Stack_t;



/* queue structure */
//struct Queue
//{
//  void *Item_p;
//
//  void (*NonEmpty)(struct Queue *Queue_p, void *Param_p);
//  void *NonEmptyParam_p;
//  void (*Empty)(struct Queue *Queue_p, void *Param_p);
//  void *EmptyParam_p;
//};

/** Type of Stack callback functions. */
typedef enum {
    STACK_EMPTY,
    STACK_NONEMPTY
} StackCallbackType_e;

/** Define of queue callback function. */
typedef void (*StackCallback_fn)(Stack_t *Stack_p, void *Param_p);

/** @} */
/** @} */
#endif /*_T_STACK_H_*/
