#ifndef INCLUSION_GUARD_R_DEBUG_H
#define INCLUSION_GUARD_R_DEBUG_H

/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/

#include <stdio.h>
#include "t_basicdefinitions.h"

/* Disable printouts if debug subsystem is disabled */
#ifndef ENABLE_DEBUG
#define DISABLE_PRINT_ALL_
#endif

// Disable flags ( "command line #undef" )
#ifdef DISABLE_PRINT_A_
#undef PRINT_A_
#endif
#ifdef DISABLE_PRINT_B_
#undef PRINT_B_
#endif
#ifdef DISABLE_PRINT_C_
#undef PRINT_C_
#endif
#ifdef DISABLE_PRINT_ALL_
#undef PRINT_C_
#undef PRINT_B_
#undef PRINT_A_
#endif

#if defined(PRINT_A_)
#define PRINT_LEVEL_A_
#endif

#if defined(PRINT_B_)
#define PRINT_LEVEL_B_
#endif

#if defined(PRINT_C_)
#define PRINT_LEVEL_C_
#endif

#if defined(PRINT_A_) || defined(PRINT_B_)
#define PRINT_LEVEL_A_B_
#endif

#if defined(PRINT_A_) || defined(PRINT_C_)
#define PRINT_LEVEL_A_C_
#endif

#if defined(PRINT_B_) || defined(PRINT_C_)
#define PRINT_LEVEL_B_C_
#endif

#if defined(PRINT_A_) || defined(PRINT_B_) || defined(PRINT_C_)
#define PRINT_LEVEL_A_B_C_
#endif

#define A_(_PAR_)
#define A_ASSERT_(exp)
#define B_(_PAR_)
#define B_ASSERT_(exp)
#define C_(_PAR_)
#define C_ASSERT_(exp)
#define A_B_(p)
#define A_B_ASSERT_(bool)
#define A_C_(p)
#define A_C_ASSERT_(bool)
#define B_C_(p)
#define B_C_ASSERT_(bool)
#define A_B_C_(p)
#define A_B_C_ASSERT_(bool)

#ifdef _WIN32
//#pragma message("COMPILING FOR WIN32")
#ifdef __cplusplus
extern "C"
{
#endif
    int _TraceEnter(int Level);
    void _TraceExit(int Level);
    void _TraceAssert(void *, void *, unsigned);
#ifdef __cplusplus
} // extern "C" {
#endif

#define _ABCTRACE(_PAR_, _ABC_) if (_TraceEnter(_ABC_)) {_PAR_; _TraceExit(_ABC_);}
#define _ASSERT(exp) (void)( (exp) || (_TraceAssert(#exp, __FILE__, __LINE__), 0) )

#ifdef PRINT_A_
#undef A_
#define A_(_PAR_) _ABCTRACE(_PAR_, 'A')
#undef A_ASSERT_
#define A_ASSERT_(exp) _ASSERT(exp)
#endif
#ifdef PRINT_B_
#undef B_
#define B_(_PAR_) _ABCTRACE(_PAR_, 'B')
#undef B_ASSERT_
#define B_ASSERT_(exp) _ASSERT(exp)
#endif
#ifdef PRINT_C_
#undef C_
#define C_(_PAR_) _ABCTRACE(_PAR_, 'C')
#undef C_ASSERT_
#define C_ASSERT_(exp) _ASSERT(exp)
#endif
#ifdef PRINT_LEVEL_A_B_
#undef A_B_
#define A_B_(_PAR_) _ABCTRACE(_PAR_, 'A')
#undef A_B__ASSERT_
#define A_B__ASSERT_(exp) _ASSERT(exp)
#endif
#ifdef PRINT_LEVEL_A_C_
#undef A_C_
#define A_C_(_PAR_) _ABCTRACE(_PAR_, 'A')
#undef A_C_ASSERT_
#define A_C_ASSERT_(exp) _ASSERT(exp)
#endif
#ifdef PRINT_LEVEL_B_C_
#undef B_C_
#define B_C_(_PAR_) _ABCTRACE(_PAR_, 'B')
#undef B_C_ASSERT_
#define B_C_ASSERT_(exp) _ASSERT(exp)
#endif
#ifdef PRINT_LEVEL_A_B_C_
#undef A_B_C_
#define A_B_C_(_PAR_) _ABCTRACE(_PAR_, 'A')
#undef A_B_C_ASSERT_
#define A_B_C_ASSERT_(exp) _ASSERT(exp)
#endif

#ifndef PRINT_LEVEL_IS_SETTABLE
// We Can remove this TRACE_IS_SETTABLE test when:
// * We do not have constructions with functions inserted in A_ macro's like: A_(void foo(void);)
// beacause  an if statement will be prefixed to the _PAR_ experssion
#ifdef PRINT_A_
#undef A_
#define A_(_PAR_) _PAR_
#undef A_ASSERT_
#define A_ASSERT_(exp) _ASSERT(exp)
#endif
#ifdef PRINT_B_
#undef B_
#define B_(_PAR_) _PAR_
#undef B_ASSERT_
#define B_ASSERT_(exp) _ASSERT(exp)
#endif
#ifdef PRINT_C_
#undef C_
#define C_(_PAR_) _PAR_
#undef C_ASSERT_
#define C_ASSERT_(exp) _ASSERT(exp)
#endif
#ifdef PRINT_LEVEL_A_B_
#undef A_B_
#define A_B_(_PAR_) _PAR_
#undef A_B__ASSERT_
#define A_B__ASSERT_(exp) _ASSERT(exp)
#endif
#ifdef PRINT_LEVEL_A_C_
#undef A_C_
#define A_C_(_PAR_) _PAR_
#undef A_C_ASSERT_
#define A_C_ASSERT_(exp) _ASSERT(exp)
#endif
#ifdef PRINT_LEVEL_B_C_
#undef B_C_
#define B_C_(_PAR_) _PAR_
#undef B_C_ASSERT_
#define B_C_ASSERT_(exp) _ASSERT(exp)
#endif
#ifdef PRINT_LEVEL_A_B_C_
#undef A_B_C_
#define A_B_C_(_PAR_) _PAR_
#undef A_B_C_ASSERT_
#define A_B_C_ASSERT_(exp) _ASSERT(exp)
#endif
#endif // PRINT_LEVEL_IS_SETTABLE

#else // _WIN32

#ifdef BSYS_ASSERT_CAUSE_DUMP
extern void p_assert_dump(const char *, unsigned int);

#define _ASSERT(expr) \
     do \
     { \
       if (!(expr)) \
       { \
         p_assert_dump(__FILE__, __LINE__); \
       } \
     } while (0)
#else
extern void p_assert_print(const char *, unsigned int);

#define _ASSERT(expr) \
     do \
     { \
       if (!(expr)) \
       { \
         p_assert_print(__FILE__, __LINE__); \
       } \
     } while (0)
#endif /* BSYS_ASSERT_CAUSE_DUMP */

#undef A_
#undef A_ASSERT_
#ifdef PRINT_A_
#define A_(p) p
#define A_ASSERT_(bool) _ASSERT(bool)
#else
#define A_(p)
#define A_ASSERT_(bool)
#endif

#undef B_
#undef B_ASSERT_
#ifdef PRINT_B_
#define B_(p) p
#define B_ASSERT_(bool) _ASSERT(bool)
#else
#define B_(p)
#define B_ASSERT_(bool)
#endif

#undef C_
#undef C_ASSERT_
#ifdef PRINT_C_
#define C_(p) p
#define C_ASSERT_(bool) _ASSERT(bool)
#else
#define C_(p)
#define C_ASSERT_(bool)
#endif

#undef A_B_
#undef A_B_ASSERT_
#ifdef PRINT_LEVEL_A_B_
#define A_B_(p) p
#define A_B_ASSERT_(bool) _ASSERT(bool)
#else
#define A_B_(p)
#define A_B_ASSERT_(bool)
#endif

#undef A_C_
#undef A_C_ASSERT_
#ifdef PRINT_LEVEL_A_C_
#define A_C_(p) p
#define A_C_ASSERT_(bool) _ASSERT(bool)
#else
#define A_C_(p)
#define A_C_ASSERT_(bool)
#endif

#undef B_C_
#undef B_C_ASSERT_
#ifdef PRINT_LEVEL_B_C_
#define B_C_(p) p
#define B_C_ASSERT_(bool) _ASSERT(bool)
#else
#define B_C_(p)
#define B_C_ASSERT_(bool)
#endif

#undef A_B_C_
#undef A_B_C_ASSERT_
#ifdef PRINT_LEVEL_A_B_C_
#define A_B_C_(p) p
#define A_B_C_ASSERT_(bool) _ASSERT(bool)
#else
#define A_B_C_(p)
#define A_B_C_ASSERT_(bool)
#endif

#endif // else _WIN32

/*
 * Obsolete TAE debugging. A_(), B_(), etc. should be used in new code.
 */
#if defined(PRINT_TA_)

#define DbgTrace1(msg)                      printf(msg)
#define DbgTrace2(msg,a1)                   printf(msg,a1)
#define DbgTrace3(msg,a1,a2)                printf(msg,a1,a2)
#define DbgTrace4(msg,a1,a2,a3)             printf(msg,a1,a2,a3)
#define DbgTrace5(msg,a1,a2,a3,a4)          printf(msg,a1,a2,a3,a4)
#define DbgTrace6(msg,a1,a2,a3,a4,a5)       printf(msg,a1,a2,a3,a4,a5)
#define DbgTrace7(msg,a1,a2,a3,a4,a5,a6)    printf(msg,a1,a2,a3,a4,a5,a6)
#define DbgTrace8(msg,a1,a2,a3,a4,a5,a6,a7) printf(msg,a1,a2,a3,a4,a5,a6,a7)

#else

#define DbgTrace1(msg)
#define DbgTrace2(msg,a1)
#define DbgTrace3(msg,a1,a2)
#define DbgTrace4(msg,a1,a2,a3)
#define DbgTrace5(msg,a1,a2,a3,a4)
#define DbgTrace6(msg,a1,a2,a3,a4,a5)
#define DbgTrace7(msg,a1,a2,a3,a4,a5,a6)
#define DbgTrace8(msg,a1,a2,a3,a4,a5,a6,a7)

#endif

#if defined(PRINT_TA_)

#define LLPRE2(expr) do{ if(expr){ printf("WARNING: ");
#define LLPOST2 }}while(0)

#define DbgIf_Warn1(expr,msg)                      LLPRE2(expr) printf(msg); LLPOST2
#define DbgIf_Warn2(expr,msg,a1)                   LLPRE2(expr) printf(msg,a1); LLPOST2
#define DbgIf_Warn3(expr,msg,a1,a2)                LLPRE2(expr) printf(msg,a1,a2); LLPOST2
#define DbgIf_Warn4(expr,msg,a1,a2,a3)             LLPRE2(expr) printf(msg,a1,a2,a3); LLPOST2
#define DbgIf_Warn5(expr,msg,a1,a2,a3,a4)          LLPRE2(expr) printf(msg,a1,a2,a3,a4); LLPOST2
#define DbgIf_Warn6(expr,msg,a1,a2,a3,a4,a5)       LLPRE2(expr) printf(msg,a1,a2,a3,a4,a5); LLPOST2
#define DbgIf_Warn7(expr,msg,a1,a2,a3,a4,a5,a6)    LLPRE2(expr) printf(msg,a1,a2,a3,a4,a5,a6); LLPOST2
#define DbgIf_Warn8(expr,msg,a1,a2,a3,a4,a5,a6,a7) LLPRE2(expr) printf(msg,a1,a2,a3,a4,a5,a6,a7); LLPOST2

#else

#define DbgIf_Warn1(expr,msg)
#define DbgIf_Warn2(expr,msg,a1)
#define DbgIf_Warn3(expr,msg,a1,a2)
#define DbgIf_Warn4(expr,msg,a1,a2,a3)
#define DbgIf_Warn5(expr,msg,a1,a2,a3,a4)
#define DbgIf_Warn6(expr,msg,a1,a2,a3,a4,a5)
#define DbgIf_Warn7(expr,msg,a1,a2,a3,a4,a5,a6)
#define DbgIf_Warn8(expr,msg,a1,a2,a3,a4,a5,a6,a7)

#endif

#define LLPRE3(expr) do{ if(expr){ printf("*FATAL* ");
#if defined(PRINT_TA_)
#define LLPOST3 LL_TerminateSystem(); }}while(0)
#else
#define LLPOST3 error(0xBABE); }}while(0)
#endif

#define DbgIf_Fatal1(expr,msg)                      LLPRE3(expr) printf(msg); LLPOST3
#define DbgIf_Fatal2(expr,msg,a1)                   LLPRE3(expr) printf(msg,a1); LLPOST3
#define DbgIf_Fatal3(expr,msg,a1,a2)                LLPRE3(expr) printf(msg,a1,a2); LLPOST3
#define DbgIf_Fatal4(expr,msg,a1,a2,a3)             LLPRE3(expr) printf(msg,a1,a2,a3); LLPOST3
#define DbgIf_Fatal5(expr,msg,a1,a2,a3,a4)          LLPRE3(expr) printf(msg,a1,a2,a3,a4); LLPOST3
#define DbgIf_Fatal6(expr,msg,a1,a2,a3,a4,a5)       LLPRE3(expr) printf(msg,a1,a2,a3,a4,a5); LLPOST3
#define DbgIf_Fatal7(expr,msg,a1,a2,a3,a4,a5,a6)    LLPRE3(expr) printf(msg,a1,a2,a3,a4,a5,a6); LLPOST3
#define DbgIf_Fatal8(expr,msg,a1,a2,a3,a4,a5,a6,a7) LLPRE3(expr) printf(msg,a1,a2,a3,a4,a5,a6,a7); LLPOST3



// Map TA calls
#define DbgTrace_HexDump(pData,nLength) BSYS_HexDump(pData,nLength,0)

// Prototypes

void BSYS_HexDump(const void *pData, int nLength, boolean DumpWithOffset);

#endif // INCLUSION_GUARD_R_DEBUG_H
