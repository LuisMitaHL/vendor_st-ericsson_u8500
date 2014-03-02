/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/**
 \if INCLUDE_IN_HTML_ONLY
 \file  Profiler.c
 \brief
 \ingroup Miscellaneous
 \endif
*/
#include "Profiler.h"

void inline
Profiler_Update(
CpuCycleProfiler_ts *ptr_CPUProfilerData)
{
    uint32_t    u32_CurrentCycles;

    STOP_PROFILER();

    u32_CurrentCycles = GET_CPU_CYCLES();

    if(ptr_CPUProfilerData == NULL)
           return;

    ptr_CPUProfilerData->u32_CurrentCycles = u32_CurrentCycles;
    if (0 != ptr_CPUProfilerData->u32_NoOfIterations)
    {
        ptr_CPUProfilerData->u32_MinimumCycle = FMIN(ptr_CPUProfilerData->u32_MinimumCycle, u32_CurrentCycles);
        ptr_CPUProfilerData->u32_MaximumCycle = FMAX(ptr_CPUProfilerData->u32_MaximumCycle, u32_CurrentCycles);
        ptr_CPUProfilerData->u32_AverageCycle = (uint32_t)
            (
                (
                    ((uint64_t) (ptr_CPUProfilerData->u32_NoOfIterations) * ptr_CPUProfilerData->u32_AverageCycle) +
                    u32_CurrentCycles
                ) / (ptr_CPUProfilerData->u32_NoOfIterations + 1)
            );
    }
    else
    {
        ptr_CPUProfilerData->u32_MinimumCycle = u32_CurrentCycles;
        ptr_CPUProfilerData->u32_MaximumCycle = u32_CurrentCycles;
        ptr_CPUProfilerData->u32_AverageCycle = u32_CurrentCycles;
    }


    ptr_CPUProfilerData->u32_NoOfIterations++;
}

