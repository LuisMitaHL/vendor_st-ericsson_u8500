/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <tu_ost_test/adder.nmf>
#include <inc/type.h>

// Must be included before OpenSystemTrace_ste.h
#define MY_OSTTRACE_LOG_TAG "TU_OST_TEST_ADDER"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "osttrace_test_tu_ost_test1_common_nmf_mmdsp_tu_ost_test_adder_src_adderTraces.h"
#endif

typedef struct {
	t_uint32 parentHandle;
	t_uint16 traceEnable;
	t_uint32 dspAddr;
} TraceInfo_t;


typedef  struct Trace {
	t_uint16      mId1;
	TraceInfo_t * mTraceInfoPtr;
} TRACE_t;

//static unsigned short aTraceEnable = &adder_aTraceEnable;
//static unsigned long aParentHandle = &adder_aParentHandle;
static unsigned short aTraceEnable;
static unsigned long aParentHandle;

static unsigned int aThisHandle;

TraceInfo_t TraceInfo;
TRACE_t MyThis;
TRACE_t *this;
#define MY_MAX_SIZE 4 
#define MY_BIG_SIZE 256
unsigned int test_array[MY_MAX_SIZE];
t_uint8 big_arr_int8[MY_BIG_SIZE];
t_uint24 big_arr_int24[MY_BIG_SIZE];


/***************************************************************************/
/*
 * Provide name : constructor
 * Interface    : controller.constructor
 *
 */
/***************************************************************************/
t_nmf_error METH(construct)(void)
{
	unsigned int i;

	// Nothing to do in construct
	TraceInfo.traceEnable = 0xFFFF;
	TraceInfo.parentHandle = 0x16171819ul;
	/* Do not care for our usage */
	TraceInfo.dspAddr = 0;
	this = &MyThis;
	this->mId1 =  *((unsigned int *)1);
	this->mTraceInfoPtr = &TraceInfo;

	for ( i=0; i<MY_BIG_SIZE; i++) { 
		big_arr_int8[i] = i; 
	}
	for ( i=0; i<MY_BIG_SIZE; i++) { 
		big_arr_int24[i] = 4096 + i; 
	}


    return NMF_OK;

}


/***************************************************************************/
/*
 * Provide name : input
 * Interface    : api.add
 * Param        : a
 * Param        : b
 *
 * Add params a and b
 * Provide result to the output interface
 */
/***************************************************************************/
#ifdef  __mode16__
#pragma force_dcumode
#endif
void METH(add)(t_uint32 a, t_uint32 b)
{
	volatile unsigned long i;
	unsigned int test1 = 0x5; 
	unsigned int test2 = 0x700; 
#ifdef __mode16__	
	unsigned int test3 = 0x9000u; 
	unsigned int test4 = 0xBBBBu; 	
#else	
	unsigned int test3 = 0x90000; 
	unsigned int test4 = 0xBBBBBBu; 
#endif

#ifdef __mode16__	
	unsigned long wait_loop = 1000ul; 
#else
	unsigned long wait_loop = 100000u; 
#endif


	OstTraceInt2 (TRACE_DEBUG, "Test OstTraceInt2: %x %x", test1, test2);
	OstTraceInt3 (TRACE_DEBUG, "Test OstTraceInt3: %x %x %x", test1, test2, test3);
	OstTraceInt4 (TRACE_DEBUG, "Test OstTraceInt4: %x %x %x %x", test1, test2, test3, test4);

	OstTraceFiltInst0 (TRACE_DEBUG,  "Test OstTraceFiltInst0:");
	OstTraceFiltInst1 (TRACE_DEBUG,  "Test OstTraceFiltInst1: %x", test1);
	OstTraceFiltInst2 (TRACE_DEBUG,  "Test OstTraceFiltInst2: %x %x", test1, test2);
	OstTraceFiltInst3 (TRACE_DEBUG,  "Test OstTraceFiltInst3: %x %x %x", test1, test2, test3);
	OstTraceFiltInst4 (TRACE_DEBUG,  "Test OstTraceFiltInst4: %x %x %x %x", test1, test2, test3, test4);

	OstTraceFiltStatic0 (TRACE_DEBUG,  "Test OstTraceFiltStatic0:", this);
	OstTraceFiltStatic1 (TRACE_DEBUG,  "Test OstTraceFiltStatic1: %x", this, test1);
	OstTraceFiltStatic2 (TRACE_DEBUG,  "Test OstTraceFiltStatic2: %x %x", this, test1, test2);
	OstTraceFiltStatic3 (TRACE_DEBUG,  "Test OstTraceFiltStatic3: %x %x %x", this, test1, test2, test3);
	OstTraceFiltStatic4 (TRACE_DEBUG,  "Test OstTraceFiltStatic4: %x %x %x %x", this, test1, test2, test3, test4);
	


	OstTraceFiltInst1 (TRACE_FLOW,  "Adder output: %d", (unsigned int)(a+b));
	
	for ( i=0; i<MY_MAX_SIZE; i++ ) 
	{ 
		test_array[i] = i; 
	}


	OstTraceArray( TRACE_DEBUG,  "Test OstTraceArray: %{int8[]}", (unsigned char*)&test_array, MY_MAX_SIZE ); 
	OstTraceArray( TRACE_DEBUG,  "Test OstTraceArray 8-bit integers with size 211: %{int8[]}", (unsigned char*)&big_arr_int8[0], 211); 
	OstTraceFiltStaticData( TRACE_DEBUG,  "Test OstTraceFiltStaticData 8-bit integers with size 211: %{int8[]}", this, (unsigned char*)&big_arr_int8[0], 211); 

	OstTraceArray( TRACE_DEBUG,  "Test OstTraceArray 24-bit integers with size 211: %{int32[]}", (unsigned int*)&big_arr_int24[0], 211); 
	OstTraceFiltStaticData( TRACE_DEBUG,  "Test OstTraceFiltStaticData 24-bit integers with size 211: %{int32[]}", this, (unsigned int*)&big_arr_int24[0], 211); 
	
	for (i=0;i<wait_loop;i++);
	output.oper((a+b));
}

void METH(ConfigureTrace)(t_uint16 aTraceFilter)
{
	this->mTraceInfoPtr->traceEnable = aTraceFilter;
}

void METH(ConfigureParentHandle)(t_uint32 aHandle)
{
	this->mTraceInfoPtr->parentHandle = aHandle;
}

