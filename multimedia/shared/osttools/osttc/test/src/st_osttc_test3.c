#define MY_OSTTRACE_LOG_TAG "TEST3"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "osttc_test_src_st_osttc_test3Traces.h"
#endif

void main(void)
{

  // Test OstTraceIntx API
  OstTraceIntFloat1 (TRACE_ERROR,       "ST_OSTTC_TEST2 TRACE_ERROR: OstTraceInt1: %d", test);
  OstTraceIntFloat2 (TRACE_WARNING,     "ST_OSTTC_TEST2 TRACE_WARNING: OstTraceInt2: %d %d", test, test);
  OstTraceIntFloat3 (TRACE_MY_GROUP_1,  "ST_OSTTC_TEST2 TRACE_MY_GROUP_6: OstTraceInt3: %d %d %d", test, test, test);
  OstTraceIntFloat4 (TRACE_MY_GROUP_2,  "ST_OSTTC_TEST2 TRACE_MY_GROUP_7: OstTraceInt4: %d %d %d %d", test, test, test, test);

  // Test OstTraceFiltStaticx API
  OstTraceFiltInstFloat1 (TRACE_OMX_API,  "ST_OSTTC_TEST3 TRACE_OMX_API: OstTraceFiltStatic1: %f", test);
  OstTraceFiltInstFloat2 (TRACE_OMX_API,  "ST_OSTTC_TEST3 TRACE_OMX_API: OstTraceFiltStatic2: %f %f", test, test);
  OstTraceFiltInstFloat3 (TRACE_OMX_API,  "ST_OSTTC_TEST3 TRACE_OMX_API: OstTraceFiltStatic3: %f %f %f", test, test, test);
  OstTraceFiltInstFloat4 (TRACE_OMX_API,  "ST_OSTTC_TEST3 TRACE_OMX_API: OstTraceFiltStatic4: %f %f %f %f", test, test, test, test);

  // Test OstTraceFiltStaticx API
  OstTraceFiltStaticFloat1 (TRACE_DEBUG,  "ST_OSTTC_TEST2 TRACE_OMX_API: OstTraceFiltStatic1: %f", test);
  OstTraceFiltStaticFloat2 (TRACE_DEBUG,  "ST_OSTTC_TEST2 TRACE_OMX_API: OstTraceFiltStatic2: %f %f", test, test);
  OstTraceFiltStaticFloat3 (TRACE_DEBUG,  "ST_OSTTC_TEST2 TRACE_OMX_API: OstTraceFiltStatic3: %f %f %f", test, test, test);
  OstTraceFiltStaticFloat4 (TRACE_DEBUG,  "ST_OSTTC_TEST2 TRACE_OMX_API: OstTraceFiltStatic4: %f %f %f %f", test, test, test, test);

}
