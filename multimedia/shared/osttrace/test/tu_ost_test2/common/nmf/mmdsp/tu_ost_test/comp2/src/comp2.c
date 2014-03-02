/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <tu_ost_test/comp2.nmf>
#include <inc/type.h>
#include "traceinit.idt"

// Must be included before OpenSystemTrace_ste.h
#define MY_OSTTRACE_LOG_TAG "TU_OST_TEST_COMP2"

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "osttrace_test_tu_ost_test2_common_nmf_mmdsp_tu_ost_test_comp2_src_comp2Traces.h"
#endif

typedef volatile __MMIO struct {
  t_uword mode;
  t_uword control;
  t_uword way;
  t_uword line;
  t_uword command;
  t_uword status;
  t_uword cptr1l;
  t_uword cptr1h;
  t_uword cptr2l;
  t_uword cptr2h;
  t_uword cptr3l;
  t_uword cptr3h;
  t_uword cptrsel;
} t_dcache_regs;

#define MMDSP_FLUSH_DATABUFFER() {*(volatile __MMIO t_uword *)0xF804 = 1;}
#define MMDSP_FLUSH_DCACHE() {t_dcache_regs * pDcache = (t_dcache_regs *)0xEC05; pDcache->command = (t_uint16)0x7U; wnop(); wnop(); MMDSP_FLUSH_DATABUFFER();}


// Mnadtory to use local structure to have SHARED pointer to avoid cache coehenrency issue
// __SHARED not supported in itf

typedef struct {
  TraceInfo_t __SHARED *mTraceInfoPtr;
  t_uint16        mId1;
} TRACE_t_dsp;


TRACE_t MyTraceInfo;
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
#pragma force_dcumode
t_nmf_error METH(construct)(void)
{
  unsigned int i;

  for ( i=0; i<MY_BIG_SIZE; i++) {
    big_arr_int8[i] = i;
  }
  for ( i=0; i<MY_BIG_SIZE; i++) {
    big_arr_int24[i] = 0x101010 + i;
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
#pragma force_dcumode
void METH(gentrace)(void)
{
  volatile unsigned long i;
  unsigned int test1 = 0x5;
  unsigned int test2 = 0x700;
  unsigned int test3 = 0x90000;
  unsigned int test4 = 0xBBBBBB;
  unsigned long wait_loop = 10000000u;

  MMDSP_FLUSH_DCACHE();

  NmfPrint0(EE_DEBUG_LEVEL_INFO, "Start STM Tracing:" MY_OSTTRACE_LOG_TAG "\n");

  NmfPrint1(EE_DEBUG_LEVEL_INFO, "parentHandle_L: 0x%x\n", (t_uint16)(this->mTraceInfoPtr->parentHandle >> 16));
  NmfPrint1(EE_DEBUG_LEVEL_INFO, "parentHandle_H: 0x%x\n", (t_uint16)(this->mTraceInfoPtr->parentHandle));
  NmfPrint1(EE_DEBUG_LEVEL_INFO, "traceEnable: 0x%x\n", this->mTraceInfoPtr->traceEnable);
  NmfPrint1(EE_DEBUG_LEVEL_INFO, "mId1: 0x%x\n", this->mId1);

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




  for ( i=0; i<MY_MAX_SIZE; i++ ) 
  { 
    test_array[i] = i; 
  }


  OstTraceArray( TRACE_DEBUG,  "Test OstTraceArray: %{hex8[]}", (unsigned char*)&test_array, MY_MAX_SIZE );
  OstTraceArray( TRACE_DEBUG,  "Test OstTraceArray 8-bit integers with size 211: %{hex8[]}", (unsigned char*)&big_arr_int8[0], 211);
  OstTraceArray( TRACE_DEBUG,  "Test OstTraceArray 24-bit integers with size 211: %{hex32[]}", (unsigned int*)&big_arr_int24[0], 211);
  OstTraceFiltStaticData( TRACE_DEBUG,  "Test OstTraceFiltStaticData 24-bit integers with size 211: %{hex32[]}", this, (unsigned int*)&big_arr_int24[0], 211);

  NmfPrint0(EE_DEBUG_LEVEL_INFO, "End STM Tracing:" MY_OSTTRACE_LOG_TAG "\n");

  output.oper(3);
}

#pragma force_dcumode
void METH(traceinit)(TRACE_t initTrace)
{
  NmfPrint0(EE_DEBUG_LEVEL_INFO, "InitTrace for:" MY_OSTTRACE_LOG_TAG "\n");
  NmfPrint1(EE_DEBUG_LEVEL_INFO, "mTraceInfoPtr: 0x%x\n", initTrace.mTraceInfoPtr);

  MyTraceInfo.mTraceInfoPtr = (TraceInfo_t *) initTrace.mTraceInfoPtr;
  MyTraceInfo.mId1 = initTrace.mId1;
  this = &MyTraceInfo;

  NmfPrint1(EE_DEBUG_LEVEL_INFO, "parentHandle_L: 0x%x\n", (t_uint16)(this->mTraceInfoPtr->parentHandle >> 16));
  NmfPrint1(EE_DEBUG_LEVEL_INFO, "parentHandle_H: 0x%x\n", (t_uint16)(this->mTraceInfoPtr->parentHandle));
  NmfPrint1(EE_DEBUG_LEVEL_INFO, "traceEnable: 0x%x\n", this->mTraceInfoPtr->traceEnable);
  NmfPrint1(EE_DEBUG_LEVEL_INFO, "mId1: 0x%x\n", this->mId1);

  output.oper(3);

}

