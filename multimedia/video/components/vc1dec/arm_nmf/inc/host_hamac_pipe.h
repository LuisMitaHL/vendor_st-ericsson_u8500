/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef HOST_HAMAC_PIPE_SPECIFIC_H
#define HOST_HAMAC_PIPE_SPECIFIC_H

/**
 * \file host_hamac_pipe.h
 * \brief hamac pipe specific implementation on host side
 */

typedef struct 
{
	t_uint16 dummy;  /* dummy field, nothing specific to VC1 in pipe */
} t_hamac_pipe_specific_info;

#include "host_hamac_pipe_generic.h"

/* PUT HERE YOUR CODEC-SPECIFIC MACROS */
/*static inline void hamac_pipe_set_write_slice_linked_list_size(t_uint16 slice_linked_list_size) {
	hamac_pipe->hamac_pipe_write_ptr->hamac_pipe_specific_info.slice_linked_list_size = slice_linked_list_size;
}

static inline t_uint16 hamac_pipe_get_write_slice_linked_list_size() {
	return hamac_pipe->hamac_pipe_write_ptr->hamac_pipe_specific_info.slice_linked_list_size;
}*/


#endif // HOST_HAMAC_PIPE_SPECIFIC_H
