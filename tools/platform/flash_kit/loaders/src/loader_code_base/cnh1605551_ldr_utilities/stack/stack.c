/*******************************************************************************
 * $Copyright ST-Ericsson 2009 $
 ******************************************************************************/

/*
 *  @addtogroup loader_utilities
 *  @{
 *    @addtogroup stack
 *    @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "r_stack_utils.h"
#include "r_debug.h"
#include "r_debug_macro.h"

/*******************************************************************************
 * Stack functions
 ******************************************************************************/

void Do_Stack_Initialize(struct Stack *Stack_p, int MaxLength)
{
    memset(Stack_p, 0, sizeof(struct Stack));
    Stack_p->Size = MaxLength;
    Stack_p->Buffer_p = (uint32 *)malloc(MaxLength * sizeof(uint32 *) + 1);
    ASSERT(NULL != Stack_p->Buffer_p);
}

void Do_Stack_Finalize(struct Stack *Stack_p)
{
    memset(Stack_p, 0, sizeof(struct Stack));
}

ErrorCode_e Do_Stack_Enqueue(struct Stack *Stack_p, uint32 Value)
{
    /* Note that the ordering of statements are important. The compiler
     * must guarantee the assignments to be atomic (which is the case
     * with properly aligned 32 bit values).
     */

    int Head = Stack_p->Head;
    boolean NonEmptyCall = (Stack_p->Head == Stack_p->Tail);

    if (Head != Stack_p->Size) {
        Stack_p->Buffer_p[Head] = Value;
        Stack_p->Head = Head + 1;

        if (NonEmptyCall && NULL != Stack_p->NonEmptyCallback) {
            Stack_p->NonEmptyCallback(Stack_p, Stack_p->NonEmptyParam_p);
        }

        return E_SUCCESS;
    } else {
        return E_FAILED_TO_STORE_IN_STACK; //E_NOSPACE
    }
}

uint32 Do_Stack_Dequeue(struct Stack *Stack_p)
{
    int Head = Stack_p->Head;
    uint32 Value = 0;

    if (Head != Stack_p->Tail) {
        Head = Head - 1;

        Value = Stack_p->Buffer_p[Head];
        Stack_p->Head = Head;

        if (Head == Stack_p->Tail && NULL != Stack_p->EmptyCallback) {
            Stack_p->EmptyCallback(Stack_p, Stack_p->EmptyParam_p);
        }
    }

    return Value;
}

StackCallback_fn Do_Stack_SetCallback(struct Stack *Stack_p,
                                      StackCallbackType_e Type,
                                      StackCallback_fn Callback,
                                      void *Param_p)
{
    StackCallback_fn OldCallback;

    if (Type == STACK_EMPTY) {
        OldCallback = Stack_p->EmptyCallback;
        Stack_p->EmptyCallback = Callback;
        Stack_p->EmptyParam_p = Param_p;
    } else {
        OldCallback = Stack_p->NonEmptyCallback;
        Stack_p->NonEmptyCallback = Callback;
        Stack_p->NonEmptyParam_p = Param_p;
    }

    return OldCallback;
}

boolean Do_Stack_IsEmpty(struct Stack *Stack_p)
{
    if (Stack_p->Tail == Stack_p->Head) {
        return TRUE;
    } else {
        return FALSE;
    }
}

boolean Do_Stack_Contains(struct Stack *Stack_p, uint32 Value)
{
    int i;

    for (i = 0; i < Stack_p->Head; i++) {
        if (Value == Stack_p->Buffer_p[i]) {
            return TRUE;
        }
    }

    return FALSE;
}

void Do_Stack_Print(Stack_t *Stack_p)
{
    int i;

    for (i = 0; i < Stack_p->Head; i++) {
        A_(printf("Stack[%d] = %d\n", i, Stack_p->Buffer_p[i]);)
    }
}

/* @} */
/* @} */
