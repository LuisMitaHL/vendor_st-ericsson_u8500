/* 
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved. 
 * This code is ST-Ericsson proprietary and confidential. 
 * Any use of the code for whatever purpose is subject to 
 * specific written permission of ST-Ericsson SA. 
 */ 
 
/**
 \if INCLUDE_IN_HTML_ONLY
 \file  Profiler.h
 \brief

 \ingroup Miscellaneous
 \endif
*/
#ifndef _PROFILER_H_
#   define _PROFILER_H_

#   include "Platform.h"
#include <measure.h>

/**
 \struct CpuCycleProfiler_ts
 \brief  Status page element to calculate CPU cycles taken by profiler
 \ingroup Miscellaneous
*/
typedef struct
{
    /// Minimum CPU cycles taken from entry to exit point \n
    /// [DEFAULT]:0
    uint32_t    u32_MinimumCycle;

    /// Maximum CPU cycles taken from entry to exit point \n
    /// [DEFAULT]:0
    uint32_t    u32_MaximumCycle;

    /// Average CPU cycles taken from entry to exit point \n
    /// [DEFAULT]:0
    uint32_t    u32_AverageCycle;

    /// No of Iteration of the procedure\n
    /// [DEFAULT]:0
    uint32_t    u32_NoOfIterations;
    /// Current cycles \n
    /// [DEFAULT]:0
    uint32_t    u32_CurrentCycles;
} CpuCycleProfiler_ts;

# define DEFAULT_VALUES_CPUCYCLEPROFILER { 0, 0, 0, 0, 0 }
#      define START_PROFILER() \
    clrcc();                   \
    startcc()
#      define STOP_PROFILER()     stopcc()
#      define GET_CPU_CYCLES()    getcc()


extern void inline Profiler_Update(CpuCycleProfiler_ts *ptr_CPUProfilerData);

//extern void GPIO_Init (void);
#endif /*_PROFILER_H_*/

