/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Dummy file used to define some conditionnal compilation flags
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef __INC_PLATFORM_OS_H
#define __INC_PLATFORM_OS_H

#undef NULL

extern void exit(int );

#ifdef __DEBUG
int logMsg(  unsigned long debug_id,   const char* function_name,
             char* arg_string,         char* arg1,
             unsigned long arg2,       unsigned long arg3,
             unsigned long arg4,       unsigned long arg5,
             unsigned long arg6,       unsigned long arg7,
             long exit_param
           );
#endif

/* to remove warning */
extern void exit(int );

/*
 * Define alignment macro
 */
#if defined(__CC_ARM)
#define ALIGN(a)                    __align(a)
#elif defined(__GNUC__)
#define ALIGN(a)                    __attribute__ ((aligned (a)))
#else
#define ALIGN(a)
#endif

/*
 * Define assertion macro
 */

/* #define HCL_ASSERT(a)                   ((a)?0:exit(0)) */

#define HCL_ASSERT(a)                   {          \
                                      if(a)   \
                                       { \
                                           exit(0); \
          }     \
                                        } 


/*
 * Define assertion macro for debug only
 */
#ifdef __DEBUG
    #define HCL_DEBUG_ASSERT(a)     HCL_ASSERT(a)
#else
    #define HCL_DEBUG_ASSERT(a)     {if(a){(void)0;}}
#endif

/*
 * Define the SPRINTF macro use inside hv_XX_debugPrintf functions
 * This routine SHALL support a format parameter with %d, %x, %s and width qualifiers
 * AND return the number of bytes written in the output string
 */
#define SPRINTF(current, max, buffer, ...) \
                              {            \
                              	if ((current + 80) > max) {break;} \
                              	current += sprintf(buffer, __VA_ARGS__);  \
                              }

/*
 * Define extended ANSI C unsigned long long type
 * could be redefine for each OS
 * typedef unsigned __int64 t_uint64;
 * typedef __int64 t_sint64;
 */
typedef unsigned long long t_uint64;
typedef signed long long t_sint64;

#endif /* __INC_PLATFORM_OS_H */
