#ifndef INCLUSION_GUARD_T_OS_H
#define INCLUSION_GUARD_T_OS_H
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/**************************************************************************
*
*     HEADER DESCRIPTION
*
*     This header file contains the types for os system
*     services. 
*    
**************************************************************************
*/

#include "t_basicdefinitions.h"

#ifdef __cplusplus
extern "C" {
#endif


#define NIL (0)


/* --------------------------------
*  --------------------------------
*  Added for remote SwBP */

    typedef uint32 SIGSELECT;
    typedef uint32 OSTICK;


    typedef uint32 PROCESS;

#define NO_PROCESS 0

/* --------------------------------
*  -------------------------------- */


/**
 * The "parent type" of all signals. Any signal can be casted 
 * to this type to extract its signal number.
 *
 * @param Primitive The signal number of the signal
 * @param sig_no    The signal number of the signal
 * @param signo     The signal number of the signal
 */
    union SIGNAL {
        SIGSELECT Primitive;
        SIGSELECT sig_no;
        SIGSELECT signo;
    };

/** Represents any signal */
    typedef union SIGNAL OS_Signal_t;



#ifdef __cplusplus
}
#endif
#endif                          /* ifndef INCLUSION_GUARD_T_OS_H */
