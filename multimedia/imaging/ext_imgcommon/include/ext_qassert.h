/********************************************************************
 * Quantum Assertions--Design by Contract(TM of ISI) facilities
// Copyright (c) 2002 Miro Samek, Palo Alto, CA.
// All Rights Reserved.
 *******************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010
*
 * ST-Ericsson added redirection of macro to proprietary asserts
*
 * Author: Steven Huet, steven.huet@stericsson.com for ST-Ericsson.
 */
 
 
 
#ifndef _EXT_QASSERT_H_
#define _EXT_QASSERT_H_

#ifdef __cplusplus
#include "ENS_DBC.h"
extern "C" {
#endif

/** NASSERT macro disables all contract validations
 * (assertions, preconditions, postconditions, and invariants).
 */
#ifndef NASSERT
                  /* callback invoked in case of assertion failure */

#define DEFINE_THIS_FILE \
   static const char THIS_FILE__[] = __FILE__

#define ASSERT(test_)\
   dbc_assert(test_,DBC_ASSERTION_FAILED,__FILE__, __LINE__)

#define REQUIRE(test_)   dbc_assert(test_,DBC_ASSERTION_FAILED,__FILE__, __LINE__)
#define ENSURE(test_)    dbc_assert(test_,DBC_ASSERTION_FAILED,__FILE__, __LINE__)
#define INVARIANT(test_) dbc_assert(test_,DBC_ASSERTION_FAILED,__FILE__, __LINE__)
#define ALLEGE(test_)    dbc_assert(test_,DBC_ASSERTION_FAILED,__FILE__, __LINE__)

#else                                           /* #ifndef NASSERT */

#define DEFINE_THIS_FILE extern const char THIS_FILE__[]
#define ASSERT(test_)
#define REQUIRE(test_)
#define ENSURE(test_)
#define INVARIANT(test_)
#define ALLEGE(test_)\
   if (test_) {      \
   }                 \
   else

#endif

#ifdef __cplusplus
}
#endif

#endif  /*_EXT_QASSERT_H_*/
