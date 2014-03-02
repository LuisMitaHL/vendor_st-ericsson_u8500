/*******************************************************************************
 * Copyright (C) ST-Ericsson SA 2011
 * License terms: 3-clause BSD license
 ******************************************************************************/
#ifndef _R_DEBUG_MACRO_H_
#define _R_DEBUG_MACRO_H_

/**
 * @addtogroup ldr_utilities
 * @{
 *   @addtogroup debug_macro
 *   @{
 */

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include "t_basicdefinitions.h"
#include "r_debug.h"
#ifdef CFG_ENABLE_PRINT_SERVER
#include "r_debug_subsystem.h"
#endif

#define IRQ_MASK 0x00000080
#define FIQ_MASK 0x00000040
#define INT_MASK (IRQ_MASK | FIQ_MASK)

#if !defined(UART_DEBUG_PORT) && defined(MACH_TYPE_STN8500)
#define UART_DEBUG_PORT 2
#endif

#if !defined(UART_DEBUG_PORT) && defined(MACH_TYPE_DB5700)
#define UART_DEBUG_PORT 0
#endif

/*******************************************************************************
 * Macro for disabling ARM interrupts. Do nothing for Windows and LINT target.
 ******************************************************************************/
#if (defined(WIN32) || defined(CFG_ENABLE_LINT_TARGET_BUILD) || defined(__linux__) || defined(__APPLE__) || defined(__CYGWIN__))
#define INT_DISABLE()
#else
#ifdef MACH_TYPE_DB5700
#define INT_DISABLE() \
  register int r0; \
  __asm("MRS     r0, CPSR"); \
  __asm("ORR     r0, r0, #INT_MASK"); \
  __asm("MSR     CPSR_c, r0");
#else
#ifdef MACH_TYPE_STN8500
#define INT_DISABLE() \
  __asm__ __volatile__ ("MRS     r0, CPSR \n\t" \
          "ORR     r0, r0, %[mask] \n\t" \
          "MSR     CPSR_c, r0" \
      : :[mask]"r"(INT_MASK): "r0", "cc");
#endif
#endif
#endif

/*******************************************************************************
 * Macros for handling fatal and  non - fatal errors.
 ******************************************************************************/
#ifndef ENABLE_DEBUG
#define VERIFY(Condition,ErrorCode)\
  do \
  { \
    if(!(Condition)) \
    { \
     ReturnValue = (ErrorCode); \
     goto ErrorExit; \
    } \
  } while(0)

#define VERIFY_NO_DEBUG(Condition,ErrorCode) VERIFY(Condition,ErrorCode)

#define VERIFY_CONDITION(Condition)\
  do \
  { \
    if(!(Condition)) \
    { \
     goto ErrorExit; \
    } \
  } while(0)

#define VERIFY_CONDITION_NO_DEBUG(Condition)    VERIFY_CONDITION(Condition)

#ifndef CFG_ENABLE_ADBG_LOADER

#define ASSERT(Condition)\
    do \
    { \
      if(!(Condition)) \
      { \
        INT_DISABLE()\
        while(1); \
      } \
    } while(0)

#define ASSERT_NO_DEBUG(Condition) ASSERT(Condition)

#else//CFG_ENABLE_ADBG_LOADER

#define ASSERT(Condition)\
    do \
    { \
      if(!(Condition)) \
      { \
      } \
    } while(0)

#define ASSERT_NO_DEBUG(Condition) ASSERT(Condition)

#endif //CFG_ENABLE_ADBG_LOADER

#else //ENABLE_DEBUG

/* VERIFY macro definition
* Verifies the condition and sets the appropriate return value
* if needed before branching the execution to ErrorExit
* This macro prints out debug info string indicating
* verification failure */
#define VERIFY(Condition,ErrorCode)\
  do \
  { \
    if(!(Condition)) \
    { \
      ReturnValue = (ErrorCode); \
        if (ErrorCode) \
          A_(printf("%s (%d): ** Verification failed! Error:(%d) **\n",__FILE__,__LINE__,(ErrorCode));) \
      goto ErrorExit; \
    } \
  } while(0)

/* NO-DEBUG version of the VERIFY macro
 * Verifies the condition and sets the appropriate return value
 * if needed before branching the execution to ErrorExit
 * This macro does not print out debug info string indicating
 * verification failure */
#define VERIFY_NO_DEBUG(Condition,ErrorCode)\
  do \
  { \
    if(!(Condition)) \
    { \
     ReturnValue = (ErrorCode); \
     goto ErrorExit; \
    } \
  } while(0)

#define VERIFY_CONDITION(Condition)\
  do \
  { \
    if(!(Condition)) \
    { \
     if(ReturnValue) \
       A_(printf("%s (%d): *** Verification failed! ReturnValue:(%d) ***\n",__FILE__,__LINE__,(ReturnValue));)\
     goto ErrorExit; \
    } \
  } while(0)

#define VERIFY_CONDITION_NO_DEBUG(Condition) \
  do \
  { \
    if(!(Condition)) \
    { \
     goto ErrorExit; \
    } \
  } while(0)

#ifndef CFG_ENABLE_ADBG_LOADER

#define ASSERT(Condition)\
    do \
    { \
      if(!(Condition)) \
      { \
        A_(printf("%s (%d): ** Assert failed **\n",__FILE__, __LINE__);)\
        INT_DISABLE()\
        while(1); \
      } \
    } while(0)

#define ASSERT_NO_DEBUG(Condition)\
    do \
    { \
      if(!(Condition)) \
      { \
        INT_DISABLE()\
        while(1); \
      } \
    } while(0)

#else//CFG_ENABLE_ADBG_LOADER

#define ASSERT(Condition)\
    do \
    { \
      if(!(Condition))\
      { \
        A_(printf("%s (%d): ** Assert failed **\n",__FILE__, __LINE__);)\
      } \
    } while(0)

#define ASSERT_NO_DEBUG(Condition)\
    do \
    { \
    } while(0)

#endif //CFG_ENABLE_ADBG_LOADER

#endif //ENABLE_DEBUG

/** @} */
/** @} */
#endif /*_R_DEBUG_MACRO_H_*/
