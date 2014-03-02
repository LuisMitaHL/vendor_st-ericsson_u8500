/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef HOST_HAMAC_PIPE_H
#define HOST_HAMAC_PIPE_H

/**
 * \file host_hamac_pipe.h
 * \brief hamac pipe specific implementation on host side
 */

typedef struct 
{
	t_uint16 slice_linked_list_size;
	t_uint16 slices;
	t_uint16 *mb_slice_map;
#ifdef NO_HAMAC    
	t_block_info *b_info;
#else
	t_macroblock_info *b_info;
#endif
  t_uint8* bits_intermediate_buffer;   /* for High profile CABAC */
} t_hamac_pipe_specific_info;

#include "host_hamac_pipe_generic.h"

#endif // HOST_HAMAC_PIPE_H
