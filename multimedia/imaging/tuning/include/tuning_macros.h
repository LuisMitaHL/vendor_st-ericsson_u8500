/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _TUNING_MACROS_H_
#define _TUNING_MACROS_H_

#if defined(TUNING_STANDALONE_TEST)
#include <stdio.h>
#define IN0(fmt)                    printf("[>%s]\n",__func__)
#define OUT0(fmt)                   printf("[<%s]\n",__func__)
#define OUTR(fmt,a)                 printf("[<%s] (ret=%d)\n",__func__,(int)a)
#define MSG0(fmt)                   printf("  [%s] "fmt,__func__)
#define MSG1(fmt,a)                 printf("  [%s] "fmt,__func__,a)
#define MSG2(fmt,a,b)               printf("  [%s] "fmt,__func__,a,b)
#define MSG3(fmt,a,b,c)             printf("  [%s] "fmt,__func__,a,b,c)
#define MSG4(fmt,a,b,c,d)           printf("  [%s] "fmt,__func__,a,b,c,d)
#define MSG5(fmt,a,b,c,d,e)         printf("  [%s] "fmt,__func__,a,b,c,d,e)
#define MSG6(fmt,a,b,c,d,e,f)       printf("  [%s] "fmt,__func__,a,b,c,d,e,f)
#define MSG7(fmt,a,b,c,d,e,f,g)     printf("  [%s] "fmt,__func__,a,b,c,d,e,f,g)
#define MSG8(fmt,a,b,c,d,e,f,g,h)   printf("  [%s] "fmt,__func__,a,b,c,d,e,f,g,h)
#define MSG9(fmt,a,b,c,d,e,f,g,h,i) printf("  [%s] "fmt,__func__,a,b,c,d,e,f,g,h,i)
#else
#include "osi_trace.h"
#define MSG5(fmt,a,b,c,d,e)         MSG9("%s%s%s%s"fmt,"","","","",a,b,c,d,e)
#define MSG6(fmt,a,b,c,d,e,f)       MSG9("%s%s%s"fmt,"","","",a,b,c,d,e,f)
#define MSG7(fmt,a,b,c,d,e,f,g)     MSG9("%s%s"fmt,"","",a,b,c,d,e,g)
#define MSG8(fmt,a,b,c,d,e,f,g,h)   MSG9("%s"fmt,"",a,b,c,d,e,f,g,h)
#endif

// ASSERT is made available in DEBUG mode and not in standalone tests
#if defined(TUNING_ASSERT_ENABLED) && !defined(TUNING_STANDALONE_TEST)
#if !defined(__SYMBIAN32__)
#include <assert.h>
#define TUNING_ASSERT(c) assert(c)
#else
#define TUNING_ASSERT(c)
#endif // __SYMBIAN32__
#else
#define TUNING_ASSERT(c)
#endif // TUNING_ASSERT_ENABLED

// TUNING_TRACE_FUNCTION enables tracing debug messages
#if defined(TUNING_TRACE_ALL) || defined (TUNING_TRACE_FUNCTION)
#define TUNING_FUNC_IN0()         IN0("\n")
#define TUNING_FUNC_OUT0()        OUT0("\n")
#define TUNING_FUNC_OUTR(a)       OUTR("",(int)a)
#else
#define TUNING_FUNC_IN0()
#define TUNING_FUNC_OUT0()
#define TUNING_FUNC_OUTR(a)
#endif

// TUNING_TRACE_INFO enables tracing debug messages
#if defined(TUNING_TRACE_ALL) || defined (TUNING_TRACE_INFO)
#define TUNING_INFO_MSG0(a)                   MSG0(a)
#define TUNING_INFO_MSG1(a,b)                 MSG1(a,b)
#define TUNING_INFO_MSG2(a,b,c)               MSG2(a,b,c)
#define TUNING_INFO_MSG3(a,b,c,d)             MSG3(a,b,c,d)
#define TUNING_INFO_MSG4(a,b,c,d,e)           MSG4(a,b,c,d,e)
#define TUNING_INFO_MSG5(a,b,c,d,e,f)         MSG5(a,b,c,d,e,f)
#define TUNING_INFO_MSG6(a,b,c,d,e,f,g)       MSG6(a,b,c,d,e,f,g)
#define TUNING_INFO_MSG7(a,b,c,d,e,f,g,h)     MSG7(a,b,c,d,e,f,g,h)
#define TUNING_INFO_MSG8(a,b,c,d,e,f,g,h,i)   MSG8(a,b,c,d,e,f,g,h,i)
#define TUNING_INFO_MSG9(a,b,c,d,e,f,g,h,i,j) MSG9(a,b,c,d,e,f,g,h,i,j)
#else
#define TUNING_INFO_MSG0(a)
#define TUNING_INFO_MSG1(a,b)
#define TUNING_INFO_MSG2(a,b,c)
#define TUNING_INFO_MSG3(a,b,c,d)
#define TUNING_INFO_MSG4(a,b,c,d,e)
#define TUNING_INFO_MSG5(a,b,c,d,e,f)
#define TUNING_INFO_MSG6(a,b,c,d,e,f,g)
#define TUNING_INFO_MSG7(a,b,c,d,e,f,g,h)
#define TUNING_INFO_MSG8(a,b,c,d,e,f,g,h,i)
#define TUNING_INFO_MSG9(a,b,c,d,e,f,g,h,i,j)
#endif

// TUNING_TRACE_ERROR enables tracing error messages
#if defined(TUNING_TRACE_ALL) || defined (TUNING_TRACE_ERROR)
#define TUNING_ERROR_MSG0(a)                   MSG0(a)
#define TUNING_ERROR_MSG1(a,b)                 MSG1(a,b)
#define TUNING_ERROR_MSG2(a,b,c)               MSG2(a,b,c)
#define TUNING_ERROR_MSG3(a,b,c,d)             MSG3(a,b,c,d)
#define TUNING_ERROR_MSG4(a,b,c,d,e)           MSG4(a,b,c,d,e)
#define TUNING_ERROR_MSG5(a,b,c,d,e,f)         MSG5(a,b,c,d,e,f)
#define TUNING_ERROR_MSG6(a,b,c,d,e,f,g)       MSG6(a,b,c,d,e,f,g)
#define TUNING_ERROR_MSG7(a,b,c,d,e,f,g,h)     MSG7(a,b,c,d,e,f,g,h)
#define TUNING_ERROR_MSG8(a,b,c,d,e,f,g,h,i)   MSG8(a,b,c,d,e,f,g,h,i)
#define TUNING_ERROR_MSG9(a,b,c,d,e,f,g,h,i,j) MSG9(a,b,c,d,e,f,g,h,i,j)
#else
#define TUNING_ERROR_MSG0(a)
#define TUNING_ERROR_MSG1(a,b)
#define TUNING_ERROR_MSG2(a,b,c)
#define TUNING_ERROR_MSG3(a,b,c,d)
#define TUNING_ERROR_MSG4(a,b,c,d,e)
#define TUNING_ERROR_MSG5(a,b,c,d,e,f)
#define TUNING_ERROR_MSG6(a,b,c,d,e,f,g)
#define TUNING_ERROR_MSG7(a,b,c,d,e,f,g,h)
#define TUNING_ERROR_MSG8(a,b,c,d,e,f,g,h,i)
#define TUNING_ERROR_MSG9(a,b,c,d,e,f,g,h,i,j)
#endif

// TUNING_TRACE_PARAM enables tracing params R/W accesses
#if defined(TUNING_TRACE_ALL) || defined (TUNING_TRACE_PARAM)
#define TUNING_PARAM_MSG0(a)                   MSG0(a)
#define TUNING_PARAM_MSG1(a,b)                 MSG1(a,b)
#define TUNING_PARAM_MSG2(a,b,c)               MSG2(a,b,c)
#define TUNING_PARAM_MSG3(a,b,c,d)             MSG3(a,b,c,d)
#define TUNING_PARAM_MSG4(a,b,c,d,e)           MSG4(a,b,c,d,e)
#define TUNING_PARAM_MSG5(a,b,c,d,e,f)         MSG5(a,b,c,d,e,f)
#define TUNING_PARAM_MSG6(a,b,c,d,e,f,g)       MSG6(a,b,c,d,e,f,g)
#define TUNING_PARAM_MSG7(a,b,c,d,e,f,g,h)     MSG7(a,b,c,d,e,f,g,h)
#define TUNING_PARAM_MSG8(a,b,c,d,e,f,g,h,i)   MSG8(a,b,c,d,e,f,g,h,i)
#define TUNING_PARAM_MSG9(a,b,c,d,e,f,g,h,i,j) MSG9(a,b,c,d,e,f,g,h,i,j)
#else
#define TUNING_PARAM_MSG0(a)
#define TUNING_PARAM_MSG1(a,b)
#define TUNING_PARAM_MSG2(a,b,c)
#define TUNING_PARAM_MSG3(a,b,c,d)
#define TUNING_PARAM_MSG4(a,b,c,d,e)
#define TUNING_PARAM_MSG5(a,b,c,d,e,f)
#define TUNING_PARAM_MSG6(a,b,c,d,e,f,g)
#define TUNING_PARAM_MSG7(a,b,c,d,e,f,g,h)
#define TUNING_PARAM_MSG8(a,b,c,d,e,f,g,h,i)
#define TUNING_PARAM_MSG9(a,b,c,d,e,f,g,h,i,j)
#endif

#endif /*_TUNING_MACROS_H_*/
