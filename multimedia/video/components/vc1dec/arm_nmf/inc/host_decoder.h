/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef HOST_DEC_H
#define HOST_DEC_H

//#include "types.h"
#include "host_types.h"
#include "host_hamac_pipe.h"
#include "host_input_control.h"


typedef struct t_dec_type
{
  t_dec_buff      buf;
	t_input_control input_control;
	t_hamac_pipe    hamac_pipe;
	t_sva_buffers   sva_buffers;
	t_uint32        sva_bypass;   // auxiliary variable to handle SVA bypass (for perf investigations only)
    t_uint16        bImmediateRelease;//CR 399075
}
t_dec;


void	init_decoder(void);
void 	close_decoder(void);
 
#endif
