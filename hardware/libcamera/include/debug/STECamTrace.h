/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
*/

#ifndef _STECAMTRACE_H_
#define _STECAMTRACE_H_

/**
 * Mandatory definitons
 *    @c CAM_LOG_TAG  : will be appended in beginning of each log.
 * 
 * Optional definitions
 *    @c _CNAME_        : class name (appears before the method name in traces)
 *    @c DUMP_METHODS   : if defined, method 'enter' and 'leave' trace are produced
 *    @c DUMP_THREAD_ID : id defined, Thread id appended in each trace
 *    @c DUMP_THIS      : if defined, this(C++) appended in each trace
 * 
 * Notes
 *    Macros with the underscore character prefix are not to be used in external
 *    code.
 */

//System includes
//For resolving warnings
#include <sys/types.h>
#include <utils/Errors.h>

#include "debug_trace.h"

//internal includes
#include "STECamDynSetting.h"
#define CAM_ASSERT_COMPILE(x) void __cam_compile_time_assert(int __cam_check[(x)?1:-1])

#ifdef DBGT_PDEBUG
#undef DBGT_PDEBUG
#define DBGT_PDEBUG(fmt, args...)                        \
    do { if (DBGT_VAR & (0x4<<(DBGT_LAYER*4))) {         \
            LOGD( "%s" DBGT_INDENT(DBGT_LAYER)"%s() [%d] ."       \
                  fmt, DBGT_PREFIX, __FUNCTION__, __LINE__, ## args);            \
        } } while (0)
#endif

#ifdef DBGT_PROLOG
#undef DBGT_PROLOG
#define DBGT_PROLOG(fmt, args...)                                       \
    do { if (DBGT_VAR & (0x2<<(DBGT_LAYER*4))) {                        \
            LOGD( "%s" DBGT_INDENT(DBGT_LAYER)"> %s() [%d] "                 \
                  fmt, DBGT_PREFIX, __FUNCTION__, __LINE__, ## args);             \
        } } while (0)
#endif

#ifdef DBGT_EPILOG
#undef DBGT_EPILOG
#define DBGT_EPILOG(fmt, args...)                                       \
    do { if (DBGT_VAR & (0x2<<(DBGT_LAYER*4))) {                        \
            LOGD( "%s" DBGT_INDENT(DBGT_LAYER)"< %s() [%d] "                 \
                  fmt, DBGT_PREFIX, __FUNCTION__, __LINE__, ## args);             \
        } } while (0)
#endif

#ifdef DBGT_PTRACE
#undef DBGT_PTRACE
#define DBGT_PTRACE(fmt, args...)                       \
    do { if (DBGT_VAR & (0x1<<(DBGT_LAYER*4))) {        \
            LOGI( "%s" DBGT_INDENT(DBGT_LAYER)" %s() [%d] "   \
                  fmt, DBGT_PREFIX, __FUNCTION__, __LINE__, ## args);           \
        } } while (0)
#endif

#endif // _STECAMTRACE_H_
