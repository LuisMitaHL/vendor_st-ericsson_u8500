/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __BRC_H_
#define __BRC_H_

#include <trace/mpc_trace.h>
#include <trace/mpc_trace.c>

asm long convto16bitmode(long value)
{
	mv @{value}, @{}
	L_msli @{}, #8, @{}
	asri @{}.0, #8, @{}.0
}


asm long convfrom16bitmode(long value)
{
	mv @{value}, @{}
	asli @{}.0, #8, @{}.0
	L_lsri @{}, #8, @{}
}

t_sva_brc_eot_fifo eotFifo;

t_sva_brc_qpConstant_state *pStateQpConstant;
t_sva_brc_cbr_state *pStateCBR;
t_sva_brc_vbr_state *pStateVBR;

t_sva_brc_qpConstant_state state_qp_constant;
t_sva_brc_cbr_state state_cbr;
t_sva_brc_vbr_state state_vbr;

t_uint32 once_global =1 ;


t_sva_ec_save skipFifo[3];

t_bool isCurrentItSkip;
t_bool isCurrentStrategicSkip;

#endif //for __BRC_H_
