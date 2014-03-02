/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010, 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   atrace.h
 * \brief  Some trace macros shared by Host and Mpc code
 * \author ST-Ericsson
 */
/*****************************************************************************/

#ifndef  _AUDIOLIBS_TRACE_H_
#define  _AUDIOLIBS_TRACE_H_

//file to override OST trace macros if needed for MPC and PROXY code
//=> this header shall be put *after* OST headers

#if defined(_SIMU_) || defined(REMOVEOSTTRACE) || defined(OVERRIDE_OST_TO_NMFLOG)
#undef OstTraceInt0
#undef OstTraceInt1
#undef OstTraceInt2
#undef OstTraceInt3
#undef OstTraceInt4
#undef OstTraceFiltInst0
#undef OstTraceFiltInst1
#undef OstTraceFiltInst2
#undef OstTraceFiltInst3
#undef OstTraceFiltInst4
#endif

#if defined(_SIMU_)
#include <stdio.h>
#define OstTraceInt0(a, b) printf((b "\n"))
#define OstTraceInt1(a, b, c) printf((b "\n"), (c))
#define OstTraceInt2(a, b, c, d) printf((b "\n"), (c), (d))
#define OstTracetInt3(a, b, c, d, e) printf((b "\n"), (c), (d), (e))
#define OstTracetInt4(a, b, c, d, e, f) printf((b "\n"), (c), (d), (e), (f))
#define OstTraceFiltInst0(a, b) printf((b "\n"))
#define OstTraceFiltInst1(a, b, c) printf((b "\n"), (c))
#define OstTraceFiltInst2(a, b, c, d) printf((b "\n"), (c), (d))
#define OstTraceFiltInst3(a, b, c, d, e) printf((b "\n"), (c), (d), (e))
#define OstTraceFiltInst4(a, b, c, d, e, f) printf((b "\n"), (c), (d), (e), (f))

#elif defined(REMOVEOSTTRACE)
#define OstTraceInt0(aGroupName, aTraceText) {}
#define OstTraceInt1(aGroupName, aTraceText, aParam ) {}
#define OstTraceInt2(aGroupName, aTraceText, aParam1, aParam2 ) {}
#define OstTraceInt3(aGroupName, aTraceText, aParam1, aParam2, aParam3 ) {}
#define OstTraceInt4(aGroupName, aTraceText, aParam1, aParam2, aParam3, aParam4 ) {}
#define OstTraceFiltInst0(aGroupName, aTraceText) {}
#define OstTraceFiltInst1(aGroupName, aTraceText, aParam ) {}
#define OstTraceFiltInst2(aGroupName, aTraceText, aParam1, aParam2 ) {}
#define OstTraceFiltInst3(aGroupName, aTraceText, aParam1, aParam2, aParam3 ) {}
#define OstTraceFiltInst4(aGroupName, aTraceText, aParam1, aParam2, aParam3, aParam4 ) {}

#elif defined(OVERRIDE_OST_TO_NMFLOG)
//for debug without OST => not for production code : keep some 'loud' warning
#warning "OVERRIDING OST Traces by NMF_LOG"
#define OstTraceInt0(aGroupName, aTraceText) NMF_LOG(aTraceText"\n")
#define OstTraceInt1(aGroupName, aTraceText, aParam ) NMF_LOG(aTraceText"\n", aParam)
#define OstTraceInt2(aGroupName, aTraceText, aParam1, aParam2 ) NMF_LOG(aTraceText"\n", aParam1, aParam2)
#define OstTraceInt3(aGroupName, aTraceText, aParam1, aParam2, aParam3 ) NMF_LOG(aTraceText"\n", aParam1, aParam2, aParam3)
#define OstTraceInt4(aGroupName, aTraceText, aParam1, aParam2, aParam3, aParam4 ) NMF_LOG(aTraceText"\n", aParam1, aParam2, aParam3, aParam4)
#define OstTraceFiltInst0(aGroupName, aTraceText) NMF_LOG(aTraceText"\n")
#define OstTraceFiltInst1(aGroupName, aTraceText, aParam ) NMF_LOG(aTraceText"\n", aParam)
#define OstTraceFiltInst2(aGroupName, aTraceText, aParam1, aParam2 ) NMF_LOG(aTraceText"\n", aParam1, aParam2)
#define OstTraceFiltInst3(aGroupName, aTraceText, aParam1, aParam2, aParam3 ) NMF_LOG(aTraceText"\n", aParam1, aParam2, aParam3)
#define OstTraceFiltInst4(aGroupName, aTraceText, aParam1, aParam2, aParam3, aParam4 ) NMF_LOG(aTraceText"\n", aParam1, aParam2, aParam3, aParam4)

#endif

#endif   // _AUDIOLIBS_TRACE_H_
