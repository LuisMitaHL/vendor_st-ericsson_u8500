/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef HOST_IO_H
#define HOST_IO_H

#include "types.h"
#include "host_types.h"
#include "settings.h"
#include <stdio.h>


#ifdef __cplusplus
extern "C" {
#endif


void ReadBitstreamFile(t_sint8 *p_file_name, t_bit_buffer **p_b);
void WriteFrame(t_seq_par *p_sp, t_uint16 *p_fb, FILE *p_out_file);

#ifdef __cplusplus
}
#endif


#endif
