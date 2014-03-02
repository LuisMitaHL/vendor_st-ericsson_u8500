/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \brief   IdleTime.cpp
 * \author  ST-Ericsson
 */
/*****************************************************************************/

#include "IdleTime.h"

#include <e32base.h>
#include "e32std.h"
#include <u32hal.h>
#include <e32svr.h>

#include "inc/type.h"

#define NB_CPU (2)


static volatile int _end_flag = 0 ;
TTime After, Before;

static void _init_idle_routine(void * args)	{
    //Symbian stuff init
    CTrapCleanup* cleanup = CTrapCleanup::New() ;
	
	TInt r = UserSvr::HalFunction(EHalGroupKernel, EKernelHalLockThreadToCpu, (TAny *)args, 0);
    if (r != KErrNone) {NMF_PANIC("CPU %d - EKernelHalLockThreadToCpu error\n",(int)args) ;}

    // Idle Loop
	while (!_end_flag) {} 
	
	TTimeIntervalMicroSeconds time ;
	TInt error = RThread().GetCpuTime(time) ;
	NMF_LOG("CPU %d - Idle time = %d\n", (int)args , I64INT(time.Int64())) ;
	
    delete cleanup;
} /* _init_distribution_routine */


static TInt _createIdleThread() {
    TThreadFunction threadFunction = (TThreadFunction) _init_idle_routine;
    TInt error;
	RThread thread ;

    static t_uint32 thread_id;
    _LIT(_LIT_Thread, "IdleThread-%d");
    TBuf16<16> thread_name;
    thread_name.Format(_LIT_Thread, thread_id);

    //Creates thread that shares the parent thread heap
    error = thread.Create(thread_name, threadFunction, (TInt) 8096, NULL, (TAny*)thread_id);
    if (error != KErrNone) return error;
	
    thread.SetPriority(EPriorityAbsoluteVeryLow);
    thread.Resume();

	thread_id++ ;
    return KErrNone;
} /* _createThread */


int idle_start() {
	TInt error;

	_end_flag = 0 ;
	for (int i=0; i<NB_CPU; i++) {
		error = _createIdleThread() ;
		if (error != KErrNone) return error ;
	}
	
	Before.UniversalTime();	
	return 0 ;
}

void idle_stop() {
	_end_flag = 1 ;
	
	After.UniversalTime();
	TTimeIntervalMicroSeconds Result = After.MicroSecondsFrom(Before); 
	NMF_LOG("Running time %d us\n",I64INT(Result.Int64())) ;
}
