/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/
#ifndef _R_STACK_H_
#define _R_STACK_H_

/**
 *  @addtogroup loader_utilities
 *  @{
 *    @addtogroup stack
 *    @{
 *      This module implement functionalities for stack manipulations. It
 *      comprise most used operations for working with stacks. It offer
 *      easy to use interface functions for common stack operations like: push
 *      and pop. Beside that, this submodule offer functions for initializing
 *      and reinitializing on stack, printing and searching stack data. All
 *      this functions give complete, easy to manage interface without
 *      restriction. Other feature that must to be mentioned is possibility of
 *      using callback functions when empty and nonempty stack is detected.
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_stack_utils.h"
#include "error_codes.h"

/*******************************************************************************
 * Declaration of functions
 ******************************************************************************/
/**
 * @brief Initialize Stack structure.
 *
 * @param[in, out]  Stack_p - pointer to uninitialized Stack structure.
 * @param[in]       MaxLength - size of stack in words.
 * @return          None.
 */
void Do_Stack_Initialize(struct Stack *Stack_p,
                         int MaxLength);

/**
 * @brief Finalize(reset) Stack structure.
 *
 * @param[in, out]  Stack_p - pointer to uninitialized Stack structure.
 * @return          None.
 */
void Do_Stack_Finalize(struct Stack *Stack_p);

/**
 * @brief Push element in top of the stack.
 *
 * @long Push process is atomic.
 *
 * @param[in, out]  Stack_p - pointer to Stack structure.
 * @param[in]       Value_p - value that will be placed in head of
 *                            the stack.
 *
 * @param[in, out]  Stack_p - pointer to Stack structure.
 * @param[in]       Value_p - points to value that will be placed in head of
 *                            the stack.
 * @return          @ref E_SUCCESS - success ending.
 *
 * @return          @ref E_FAILED_TO_STORE_IN_STACK - cannot find free space in
 *                  stack.
 */
ErrorCode_e Do_Stack_Enqueue(struct Stack *Stack_p,
                             uint32 Value);

/**
 * @brief Pop element from top of the stack.
 *
 * @long Pop process is atomic.
 *
 * @param[in, out]  Stack_p - pointer to Stack structure.
 * @return          value of element that was placed last on the stack head.
 */
uint32 Do_Stack_Dequeue(struct Stack *Stack_p);

/**
 * @brief Set callback functions for push and pop operations.
 *
 * @param[in, out]  Stack_p - pointer to Stack structure.
 * @param[in]       Type - hold type of stack callback function.
 * @param[in]       Callback - function that will be set as callback.
 * @param[in]       Param_p - input parameters for callback function.
 * @return          Old (previously used) callback function.
 */
StackCallback_fn Do_Stack_SetCallback(struct Stack *Stack_p,
                                      StackCallbackType_e Type,
                                      StackCallback_fn Callback,
                                      void *Param_p);

/**
 * @brief Check if stack is empty.
 *
 * @param[in, out]  Stack_p - pointer to Stack structure.
 * @return          TRUE if stack is empty and FALSE otherwise (some valid data
 *                  exist).
 */
boolean Do_Stack_IsEmpty(struct Stack *Stack_p);

/**
 * @brief Search entire stack for specified value.
 *
 * @param[in, out]  Stack_p - pointer to Stack structure.
 * @param[in]       Value_p - value that will be searched in stack.
 * @return          TRUE if such value exsist in stack and FALSE otherwise.
 */
boolean Do_Stack_Contains(struct Stack *Stack_p,
                          uint32 Value);

/**
 * @brief Print out stack elements, starting from tail.
 *
 * @param[in, out]  Stack_p - pointer to Stack structure.
 * @return          None.
 */
void Do_Stack_Print(Stack_t *Stack_p);

/** @} */
/** @} */
#endif /*_R_STACK_H_*/
