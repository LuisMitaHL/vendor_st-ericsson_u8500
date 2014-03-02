#define MY_OSTTRACE_LOG_TAG "TEST1"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "osttc_test_src_st_osttc_test1yTraces.h"
#endif

void main(void)
{
	// Test All GROUPS
	OstTraceInt0 (TRACE_ERROR,  			"TRACE_ERROR: 				OstTraceInt0");
	OstTraceInt0 (TRACE_WARNING,  		"TRACE_WARNING: 			OstTraceInt0");
	OstTraceInt0 (TRACE_FLOW,  				"TRACE_FLOW:  				OstTraceInt0");
	OstTraceInt0 (TRACE_DEBUG,  			"TRACE_DEBUG:  				OstTraceInt0");	
	OstTraceInt0 (TRACE_API,  				"TRACE_API:  					OstTraceInt0");
	OstTraceInt0 (TRACE_OMX_API,  		"TRACE_OMX_API: 			OstTraceInt0");
	OstTraceInt0 (TRACE_OMX_BUFFER, 	"TRACE_OMX_BUFFER: 		OstTraceInt0");
	OstTraceInt0 (TRACE_RESERVED, 		"TRACE_OMX_RESERVED: 	OstTraceInt0");
	OstTraceInt0 (TRACE_MY_GROUP_0, 	"TRACE_MY_GROUP_0: 		OstTraceInt0");
	OstTraceInt0 (TRACE_MY_GROUP_1, 	"TRACE_MY_GROUP_1: 		OstTraceInt0");
	OstTraceInt0 (TRACE_MY_GROUP_2, 	"TRACE_MY_GROUP_2: 		OstTraceInt0");
	OstTraceInt0 (TRACE_MY_GROUP_3, 	"TRACE_MY_GROUP_3: 		OstTraceInt0");
	OstTraceInt0 (TRACE_MY_GROUP_4, 	"TRACE_MY_GROUP_4: 		OstTraceInt0");
	OstTraceInt0 (TRACE_MY_GROUP_5, 	"TRACE_MY_GROUP_5: 		OstTraceInt0");
	OstTraceInt0 (TRACE_MY_GROUP_6, 	"TRACE_MY_GROUP_6: 		OstTraceInt0");
	OstTraceInt0 (TRACE_MY_GROUP_7, 	"TRACE_MY_GROUP_7: 		OstTraceInt0");

	// Test OstTraceIntx API
	OstTraceInt1 (TRACE_ERROR,  			"TRACE_ERROR: 				OstTraceInt1: %d", test);
	OstTraceInt2 (TRACE_WARNING,  		"TRACE_WARNING: 			OstTraceInt2: %d %d", test, test);
	OstTraceInt3 (TRACE_MY_GROUP_6,  	"TRACE_MY_GROUP_6: 		OstTraceInt3: %d %d %d", test, test, test);
	OstTraceInt4 (TRACE_MY_GROUP_7,  	"TRACE_MY_GROUP_7: 		OstTraceInt4: %d %d %d %d", test, test, test, test);

	// Test OstTraceFiltStaticx API
	OstTraceFiltInst0 (TRACE_OMX_API,  "TRACE_OMX_API: 	OstTraceFiltStatic0:");
	OstTraceFiltInst1 (TRACE_OMX_API,  "TRACE_OMX_API: 	OstTraceFiltStatic1	: %d", test);
	OstTraceFiltInst2 (TRACE_OMX_API,  "TRACE_OMX_API: 	OstTraceFiltStatic2	: %d %d", test, test);
	OstTraceFiltInst3 (TRACE_OMX_API,  "TRACE_OMX_API: 	OstTraceFiltStatic3	: %d %d %d", test, test, test);
	OstTraceFiltInst4 (TRACE_OMX_API,  "TRACE_OMX_API: 	OstTraceFiltStatic4	: %d %d %d %d", test, test, test, test);

	// Test OstTraceFiltStaticx API
	OstTraceFiltStatic0 (TRACE_OMX_API,  "TRACE_OMX_API: 	OstTraceFiltStatic0:");
	OstTraceFiltStatic1 (TRACE_OMX_API,  "TRACE_OMX_API: 	OstTraceFiltStatic1	: %d", test);
	OstTraceFiltStatic2 (TRACE_OMX_API,  "TRACE_OMX_API: 	OstTraceFiltStatic2	: %d %d", test, test);
	OstTraceFiltStatic3 (TRACE_OMX_API,  "TRACE_OMX_API: 	OstTraceFiltStatic3	: %d %d %d", test, test, test);
	OstTraceFiltStatic4 (TRACE_OMX_API,  "TRACE_OMX_API: 	OstTraceFiltStatic4	: %d %d %d %d", test, test, test, test);
	
	// Test trace Buffer API
	OstTraceArray (TRACE_RESERVED,  "TRACE_OMX_API: 	OstTraceArray %{int8[]}", ptr, size);
	OstTraceFiltInstData(TRACE_RESERVED,  "TRACE_OMX_API: 	OstTraceFiltInstData %{int8[]}", ptr, size);
	OstTraceFiltStaticData(TRACE_RESERVED,  "TRACE_OMX_API: 	OstTraceFiltInstData %{int8[]}",aInstPtr, ptr, size);

	// Check Syntax ALL below syntax shall be parsed
	OstTraceInt0 (TRACE_ERROR,  			"TRACE_ERROR: 				OstTraceInt0");
	OstTraceInt0 ( TRACE_ERROR ,  			"TRACE_ERROR: 				OstTraceInt0");
	// OstTraceInt0 ( TRACE_ERROR ,  			"This is a commented line with //");
	/* OstTraceInt0 ( TRACE_ERROR ,  			"This is a commented line with /*"); */
	{ OstTraceInt0 ( TRACE_ERROR ,  			"TRACE_ERROR: 				OstTraceInt0 with preceding {");

  // Shall not be parsed
#define OstTraceInt0 OstTraceFiltStaticData(TRACE_RESERVED
	
}
