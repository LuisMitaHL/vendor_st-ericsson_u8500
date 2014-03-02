/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#include <rtos/common.nmf>
#include <inc/archi-wrapper.h>
#include <stddef.h>

#define DESACT_U 0xE  //1110
#define DESACT_S 0xB  //1011
#define CLEAR_U 0x2   //0010
#define CLEAR_S 0x8   //1000
#define ACT_U 0x1     //0001
#define ACT_S 0x4     //0100

void init_stackcheck_U(unsigned int stack_size, int * stack_address)
{
    ENTER_CRITICAL_SECTION

    //desactivates the check
    STACK_CHECK_CMD &= DESACT_U; //the check must be desactivated

    //sets limits
    STACK_U_LOW_LIMIT_L = (size_t)stack_address & 0xFFFFU;
    STACK_U_LOW_LIMIT_H = (size_t)stack_address >> 16;

    STACK_U_HIGH_LIMIT_L = ((size_t)stack_address + stack_size) & 0xFFFFU;
    STACK_U_HIGH_LIMIT_H = ((size_t)stack_address + stack_size) >> 16;

    //clear the memorized values before asking a new control
    STACK_CHECK_CMD |= CLEAR_U;
    STACK_CHECK_CMD &= ~CLEAR_U;

    //activate the check
    STACK_CHECK_CMD |= ACT_U;

    EXIT_CRITICAL_SECTION
}

void init_stackcheck_S(unsigned int stack_size, int * stack_address)
{
    ENTER_CRITICAL_SECTION

    //desactivates the check
    STACK_CHECK_CMD &= DESACT_S; //the check must be desactivated

    //sets limits
    STACK_S_LOW_LIMIT_L = (size_t)stack_address & 0xFFFFU;
    STACK_S_LOW_LIMIT_H = (size_t)stack_address >> 16;

    STACK_S_HIGH_LIMIT_L = ((size_t)stack_address + stack_size) & 0xFFFFU;
    STACK_S_HIGH_LIMIT_H = ((size_t)stack_address + stack_size) >> 16;

    //activates the check
    STACK_CHECK_CMD |= CLEAR_S; //clear the memorized values before asking a new control
    STACK_CHECK_CMD &= ~CLEAR_S;

    STACK_CHECK_CMD |= ACT_S; //activate the check

    EXIT_CRITICAL_SECTION
}
