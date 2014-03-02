#ifndef INCLUSION_GUARD_R_DEBUG_H
#define INCLUSION_GUARD_R_DEBUG_H
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*******************************************************************************
*
* DESCRIPTION: r_debug.h
*   Various macros meant to be used for debugging purposes.
*
******************************************************************************/

/******************************************************************************
 * Note: The general ban on preprocessor directives for conditional compilation
 * in SwBP header files is not obeyed in this file. If the ban had been obeyed,
 * this header would be unusable and its purposes none. Thus the ban is not
 * applicable in this case.
******************************************************************************/

/******************************************************************************
* Includes
******************************************************************************/

#include "t_basicdefinitions.h"


/******************************************************************************
* Macros
******************************************************************************/

#ifndef _WIN32
  // Needed for finding EMP_Printf declaration.
/*  #include "r_printserver.h"  */
  /**
   * When this header is included and we are not building for a Win32
   * environment, printf is defined as a macro, which expands to a function in
   * the print server.
   *
   * Note: the usage of printf in interrupt context is discouraged.
   */
/*  #define printf EMP_Printf */
#else                           // _WIN32
  // Win32 needs the ordinary output to stdout.
#include <stdio.h>
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
#define A_B_ASSERT_(Condition)
#define A_C_(p)
#define A_C_ASSERT_(Condition)
#define B_C_(p)
#define B_C_ASSERT_(Condition)
#define A_B_C_(p)
#define A_B_C_ASSERT_(Condition)

#ifdef _WIN32
//#pragma message("COMPILING FOR WIN32")
#ifdef __cplusplus
extern "C" {
#endif
    int _TraceEnter(
    int Level);
    void _TraceExit(
    int Level);
    void _TraceAssert(
    void *,
    void *,
    unsigned);
#ifdef __cplusplus
}                               // extern "C" {
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
    /* We can remove this TRACE_IS_SETTABLE test when: We do not have
     * constructions with functions inserted in A_ macro's like: A_(void
     * foo(void);) beacause an if statement will be prefixed to the _PAR_
     * expression.
     */
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
#endif                          // PRINT_LEVEL_IS_SETTABLE
#else                           // _WIN32

#ifdef BSYS_ASSERT_CAUSE_DUMP
extern
#ifdef __cplusplus
 "C"
#endif
void AssertDump(
    const char *FileName,
    unsigned int Line);

#define _ASSERT(expr) \
      do \
      { \
        if (!(expr)) \
        { \
          AssertDump(__FILE__, __LINE__); \
        } \
      } while (0)
#else
extern
#ifdef __cplusplus
 "C"
#endif
void AssertPrint(
    const char *FileName,
    unsigned int Line);

#define _ASSERT(expr) \
      do \
      { \
        if (!(expr)) \
        { \
          AssertPrint(__FILE__, __LINE__); \
        } \
      } while (0)
#endif                          /* BSYS_ASSERT_CAUSE_DUMP */

#undef A_
#undef A_ASSERT_
#ifdef PRINT_A_
#define A_(p) p
#define A_ASSERT_(Condition) _ASSERT(Condition)
#else
#define A_(p)
#define A_ASSERT_(Condition)
#endif

#undef B_
#undef B_ASSERT_
#ifdef PRINT_B_
#define B_(p) p
#define B_ASSERT_(Condition) _ASSERT(Condition)
#else
#define B_(p)
#define B_ASSERT_(Condition)
#endif

#undef C_
#undef C_ASSERT_
#ifdef PRINT_C_
#define C_(p) p
#define C_ASSERT_(Condition) _ASSERT(Condition)
#else
#define C_(p)
#define C_ASSERT_(Condition)
#endif

#undef A_B_
#undef A_B_ASSERT_
#ifdef PRINT_LEVEL_A_B_
#define A_B_(p) p
#define A_B_ASSERT_(Condition) _ASSERT(Condition)
#else
#define A_B_(p)
#define A_B_ASSERT_(Condition)
#endif

#undef A_C_
#undef A_C_ASSERT_
#ifdef PRINT_LEVEL_A_C_
#define A_C_(p) p
#define A_C_ASSERT_(Condition) _ASSERT(Condition)
#else
#define A_C_(p)
#define A_C_ASSERT_(Condition)
#endif

#undef B_C_
#undef B_C_ASSERT_
#ifdef PRINT_LEVEL_B_C_
#define B_C_(p) p
#define B_C_ASSERT_(Condition) _ASSERT(Condition)
#else
#define B_C_(p)
#define B_C_ASSERT_(Condition)
#endif

#undef A_B_C_
#undef A_B_C_ASSERT_
#ifdef PRINT_LEVEL_A_B_C_
#define A_B_C_(p) p
#define A_B_C_ASSERT_(Condition) _ASSERT(Condition)
#else
#define A_B_C_(p)
#define A_B_C_ASSERT_(Condition)
#endif

#endif                          // else _WIN32

/* Obsolete TAE debugging. A_(), B_(), etc. should be used in new code.
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
#define DbgTrace_HexDump(Data_p,Length) BSYS_HexDump(Data_p,Length,0)


/******************************************************************************
* Function declarations
******************************************************************************/

#ifdef __cplusplus
extern "C"
#endif
void BSYS_HexDump(
    const void *Data_p,
    int Length,
    boolean DumpWithOffset);


/******************************************************************************
* Macros moved from genutil_assert.h
******************************************************************************/

// Flags controlling assertion behaviour:
//
// DISABLE_ASSERTION_RECOVERY
// SHIPPING_BUILD
// DISABLE_SMART_ASSERT
//
/////////////////////////////////////////////////


#ifndef ASSERT_FOR_SHIPPING
#ifdef  DISABLE_ASSERTION_RECOVERY
#define ASSERT_FOR_SHIPPING(Condition, RecoverAction)
#else
#define ASSERT_FOR_SHIPPING(Condition, RecoverAction)  \
  do {                                                 \
    if (!(Condition))                                  \
    {                                                  \
      A_(printf("\n#Condition: invalid input parameter(s)\n");) \
      RecoverAction;                                   \
    }                                                  \
  }                                                    \
  while (0)
#endif
#endif



/*lint -emacro(506, ASSERT_FOR_DEBUGGING) Inhibit warning for constant boolean*/
/*lint -emacro(527, ASSERT_FOR_DEBUGGING) Inhibit warning for unreachable - as RecoverActions is unreachable*/
/*lint -emacro(774, ASSERT_FOR_DEBUGGING) Info of if always evaluating to True*/

#ifndef ASSERT_FOR_DEBUGGING
#define ASSERT_FOR_DEBUGGING(Condition, RecoverAction) \
  do                                                   \
  {                                                    \
    if (!(Condition))                                  \
    {                                                  \
      A_(printf("\n#Condition: invalid input parameter(s)\n");) \
      RecoverAction;                                   \
    }                                                  \
  }                                                    \
  while (0)
#endif



#ifndef NO_RECOVERY_ACTION
#define NO_RECOVERY_ACTION (void)0
#endif



/***** Definition of ASSERT *****/
#ifndef SMART_ASSERT
#if defined(SHIPPING_BUILD) || defined(DISABLE_SMART_ASSERT)
#define SMART_ASSERT(Condition, RecoverAction) ASSERT_FOR_SHIPPING(Condition, RecoverAction)
#else
#define SMART_ASSERT(Condition, RecoverAction) ASSERT_FOR_DEBUGGING(Condition, RecoverAction)
#endif
#endif

/***** Definition of COMPILE_TIME_ASSERT *****/

/*lint -esym(752, CompileTimeAssertSymbol) Inhibit unused declaration info*/
#ifndef COMPILE_TIME_ASSERT
/* Please do not use the COMPILE_TIME_ASSERT macro in new code: use the macros
 *
 * COMPILE_TIME_ASSERT_STATEMENT
 *
 * and
 *
 * COMPILE_TIME_ASSERT_DECL
 *
 * from r_sys.h instead. This macro will be removed in the future.
 */
#define COMPILE_TIME_ASSERT(Condition) \
  {extern int CompileTimeAssertSymbol[1/((int)(!!(Condition)))];}
#endif

/***** Definition of PRE_CONDITION *****/
#ifndef PRE_CONDITION
#if defined(SHIPPING_BUILD) || defined(DISABLE_PRE_CONDITIONS)
#define PRE_CONDITION(Condition, RecoverAction) ASSERT_FOR_SHIPPING(Condition, RecoverAction)
#else
#define PRE_CONDITION(Condition, RecoverAction) ASSERT_FOR_DEBUGGING(Condition, RecoverAction)
#endif
#endif

/***** Definition of POST_CONDITION *****/
#ifndef POST_CONDITION
#if defined(SHIPPING_BUILD) || !defined(ENABLE_POST_CONDITIONS)
#define POST_CONDITION(Condition, RecoverAction) ASSERT_FOR_SHIPPING(Condition, RecoverAction)
#else
#define POST_CONDITION(Condition, RecoverAction) ASSERT_FOR_DEBUGGING(Condition, RecoverAction)
#endif
#endif

/***** Definition of PRE_INVARIANT *****/
#ifndef PRE_INVARIANT
#if defined(SHIPPING_BUILD) || defined(DISABLE_PRE_INVARIANT)
#define PRE_INVARIANT(Condition, RecoverAction) ASSERT_FOR_SHIPPING(Condition, RecoverAction)
#else
#define PRE_INVARIANT(Condition, RecoverAction) ASSERT_FOR_DEBUGGING(Condition, RecoverAction)
#endif
#endif

/***** Definition of POST_INVARIANT *****/
#ifndef POST_INVARIANT
#if defined(SHIPPING_BUILD) || !defined(ENABLE_POST_INVARIANT)
#define POST_INVARIANT(Condition, RecoverAction) ASSERT_FOR_SHIPPING(Condition, RecoverAction)
#else
#define POST_INVARIANT(Condition, RecoverAction) ASSERT_FOR_DEBUGGING(Condition, RecoverAction)
#endif
#endif

/******************************************************************************
* No more macros from genutil_assert.h
******************************************************************************/

#endif                          // INCLUSION_GUARD_R_DEBUG_H
