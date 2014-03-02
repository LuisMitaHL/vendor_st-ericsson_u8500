/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _TCM_TRACE_H_
#define _TCM_TRACE_H_

enum e_trc_id {
 IRP_TRACE_IT_18         = 0xA2,
 IRP_TRACE_IT_19         = 0xA3,
 IRP_TRACE_IT_21         = 0xA4,
 IRP_TRACE_IT_22         = 0xA5,
 IRP_TRACE_IT_23         = 0xA6,
 IRP_TRACE_IT_18_OUT     = 0xA7,
 IRP_TRACE_IT_19_OUT     = 0xA8,
 IRP_TRACE_IT_21_OUT     = 0xA9,
 IRP_TRACE_IT_22_OUT     = 0xAA,
 IRP_TRACE_IT_23_OUT     = 0xAB,

 IRP_TRACE_PE_WRITE_LIST = 0xC1,
 IRP_TRACE_PE_WRITE_LIST_OUT = 0xC2,
 MEM_TRACE_PE_WRITE      = 0xC3,
 IRP_TRACE_PE_WRITE_OUT  = 0xC4,
 IRP_TRACE_READ_PE       = 0xC5,
 IRP_TRACE_READ_PE_OUT   = 0xC6,
 IRP_TRACE_PE_READ_LIST  = 0xC7,
 IRP_TRACE_PE_READ_LIST_OUT = 0xC8,
 IRP_TRACE_INIT_ISP      = 0xC9,
 IRP_TRACE_INIT_ISP_OUT  = 0xCA,
 IRP_TRACE_PE_WAIT       = 0xCB,
 IRP_TRACE_PE_WAIT_OUT   = 0xCC,

 HW_INIT                 = 0xD1,
 HW_INIT_OUT             = 0xD2,
 HW_DEINIT               = 0xD3,
 HW_DEINIT_OUT           = 0xD4,

 GRB_TRACE_IT_16         = 0xB1,
 GRB_TRACE_IT_17         = 0xB2,
 GRB_TRACE_IT_24         = 0xB3,
 GRB_TRACE_IT_25         = 0xB4,
 GRB_TRACE_IT_16_OUT     = 0xB5,
 GRB_TRACE_IT_17_OUT     = 0xB6,
 GRB_TRACE_IT_24_OUT     = 0xB7,
 GRB_TRACE_IT_25_OUT     = 0xB8,
 GRB_TRACE_EXEC          = 0xBA,
 GRB_TRACE_EXEC_OUT      = 0xBB,
 GRB_TRACE_EXEC2         = 0xBC,
 GRB_TRACE_EXEC2_OUT     = 0xBD
};

#ifdef __DEBUG_TCM_TRACE
#define MEM_TRACE_SIZE 16
volatile t_uint16 debug_table[MEM_TRACE_SIZE] = {0xDEADU,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0xDEADU};
volatile t_uint16 debug_table_ix  = 1;
#define MEM_TRACE(a) debug_table[debug_table_ix++]=a; if(debug_table_ix==MEM_TRACE_SIZE-1)debug_table_ix=1; debug_table[debug_table_ix]=0xF00DU;

#else
/*t_uint16 volatile * p_debug_table    = (t_uint16*)0x405;
t_uint16 volatile * p_debug_table_ix = (t_uint16*)0x415;
#define MEM_TRACE(a) *(p_debug_table+*p_debug_table_ix)=a; (*p_debug_table_ix)++; if(*p_debug_table_ix==15)*p_debug_table_ix=1; *(p_debug_table+*p_debug_table_ix)=0xF00DU; */
#define MEM_TRACE(a)
#endif

#endif /* _TCM_TRACE_H_ */
