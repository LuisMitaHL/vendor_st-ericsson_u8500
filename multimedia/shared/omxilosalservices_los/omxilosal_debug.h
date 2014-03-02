#ifndef DEBUG_TRACE_OMXILOSAL
#define DEBUG_TRACE_OMXILOSAL

#include <stdlib.h>

#ifndef __PRETTY_FUNCTION__
  #define __PRETTY_FUNCTION__ __FUNCTION__
#endif

/* Unconditional, logged as error, function with signature only */
#ifndef DBGT_ERROR
   #define DBGT_ERROR(fmt, args...)                                \
       LOGE( "%s" "! %s "                   \
            fmt, DBGT_PREFIX, __PRETTY_FUNCTION__, ## args)
#endif

#ifdef DBGT_CONFIG_DEBUG
#include <cutils/properties.h>

#define DBGT_INDENT_0 " "
#define DBGT_INDENT_1 "  "
#define DBGT_INDENT_2 "   "
#define DBGT_INDENT_3 "    "
#define DBGT_INDENT_4 "     "
#define DBGT_INDENT_5 "      "
#define DBGT_INDENT_6 "       "
#define DBGT_INDENT_(i) DBGT_INDENT_##i
#define DBGT_INDENT(i)  DBGT_INDENT_(i)

#ifndef DBGT_TAG
  #define DBGT_TAG "DBGT"
#endif


#define GET_PROPERTY(key, value, default_value) \
    char value[PROPERTY_VALUE_MAX];             \
    property_get(key, value, default_value);
     
#define DBGT_TRACE_NAME "debug.osal.trace"

/* Traces can be enabled dynmically by using following command:
setprop debug.osal.trace 0xff
*/

#define STR(x) x


#ifndef DBGT_PTRACE
#define DBGT_PTRACE(fmt, args...)                                   \
        do {                                                        \
            int mDBGTvar = 0;                                       \
            GET_PROPERTY(STR(DBGT_TRACE_NAME), value, "0");          \
            mDBGTvar = strtoul(value, NULL, 16);		        	\
            if (mDBGTvar & (0x1<<(DBGT_LAYER*4))) {	            	\
				LOGI( "%s" DBGT_INDENT(DBGT_LAYER)		        	\
					  fmt, DBGT_PREFIX, ## args);		        	\
			}                                                       \
        } while (0)
#endif
#endif

#endif
