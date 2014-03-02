/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <tu_ost_test/dec_one.nmf>
#include <inc/type.h>


/***************************************************************************/
/*
 * Provide name : input
 * Interface    : api.uniop
 * Param        : d
 *
 * Remove one to the input
 */
/***************************************************************************/
void METH(oper)(t_uint32 d)
{
	output.oper((d-1));
}





