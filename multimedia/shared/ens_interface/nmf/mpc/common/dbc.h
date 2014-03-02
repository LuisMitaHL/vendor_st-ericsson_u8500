/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   dbc.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef DBC_H_
#define DBC_H_

#ifdef NDBC

  #define ASSERT(a)
  #define PRECONDITION(a)
  #define POSTCONDITION(a)
  #define INVARIANT(a)
#else

typedef enum {
    DBC_ASSERTION_FAILED,
    DBC_PRECONDITION_FAILED,
    DBC_POSTCONDITION_FAILED,
    DBC_INVARIANT_FAILED
} DBCError_e;

  #define DBC_ASSERT(a, error) \
    { \
        if (!(a)) { \
            Panic(USER_PANIC, error); \
        } \
    }

  #define ASSERT(a) DBC_ASSERT(a, __LINE__)
  #define PRECONDITION(a) DBC_ASSERT(a, __LINE__)
  #define POSTCONDITION(a) DBC_ASSERT(a, __LINE__)
  #define INVARIANT(a) DBC_ASSERT(a, __LINE__)

#endif
#endif
