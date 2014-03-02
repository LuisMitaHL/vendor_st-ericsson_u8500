/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include <tu_ost_test/dec_arm.nmf>
// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"

#ifdef OST_TRACE_COMPILER_IN_USE
#include "osttrace_test_tu_ost_test1_common_nmf_arm_tu_ost_test_dec_arm_src_dec_armTraces.h"
#endif




/***************************************************************************/
/*
 * Provide name : constructor
 * Interface    : controller.constructor
 *
 * Nothing to do
 */
/***************************************************************************/
t_nmf_error METH(construct)()
{
	aTraceEnable = 0xFFFF;
	aParentHandle = 0x300;
	aInstance = (unsigned int) this;
    return NMF_OK;
}

/***************************************************************************/
/*
 * Provide name : input
 * Interface    : api.uniop
 * Param        : d
 *
 * Remove one to the input
 */
/***************************************************************************/

void METH(oper)(t_uint32 d, t_uint8 index)
{
	if (! IS_NULL_INTERFACE(output[index], oper)) 
	{
		OstTraceFiltInst1 (TRACE_FLOW, "Dec Arm output: %d", d-delta);

		output[index].oper((d-delta));
	}
}


void METH(ConfigureTrace)(t_uint16 aTraceFilter)
{
	aTraceEnable = aTraceFilter;
}

void METH(ConfigureParentHandle)(t_uint32 aHandle)
{
	aParentHandle = aHandle;
}


void METH(Aoper)(t_uint32 d)
{
}


