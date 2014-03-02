/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#include "host_hamac_pipe_generic.hpp"

inline void hamac_pipe_set_write_slice_linked_list_size(t_uint16 slice_linked_list_size) {
	hamac_pipe->hamac_pipe_write_ptr->hamac_pipe_specific_info.slice_linked_list_size = slice_linked_list_size;
}

inline t_uint16 hamac_pipe_get_write_slice_linked_list_size() {
	return hamac_pipe->hamac_pipe_write_ptr->hamac_pipe_specific_info.slice_linked_list_size;
}

inline void hamac_pipe_set_slices(t_uint16 slices) {
	hamac_pipe->hamac_pipe_write_ptr->hamac_pipe_specific_info.slices = slices;
}

inline t_uint16 hamac_pipe_get_slices() {
	return hamac_pipe->hamac_pipe_read_ptr->hamac_pipe_specific_info.slices;
}

inline t_uint16 *hamac_pipe_get_write_mb_slice_map() {
	return hamac_pipe->hamac_pipe_write_ptr->hamac_pipe_specific_info.mb_slice_map;
}

inline t_uint8 *hamac_pipe_get_write_bits_intermediate_buffer(){
	return hamac_pipe->hamac_pipe_write_ptr->hamac_pipe_specific_info.bits_intermediate_buffer;
}

inline t_block_info *hamac_pipe_get_write_b_info() {
	return hamac_pipe->hamac_pipe_write_ptr->hamac_pipe_specific_info.b_info;
}

inline t_sint32 hamac_pipe_is_empty() {
 return hamac_pipe->pipe_is_empty;
}

inline t_sint16 hamac_pipe_get_read() {
 return hamac_pipe->hamac_pipe_read;
}

inline void hamac_pipe_set_read(t_sint16 pipe_read) {
 hamac_pipe->hamac_pipe_read = pipe_read;
 hamac_pipe->pipe_is_empty = 0;
}

inline t_uint16 aux_hamac_pipe_is_empty() {
  if(hamac_pipe->aux_hamac_pipe_read_pos == hamac_pipe->aux_hamac_pipe_write_pos) {
    return 1; 
  } else {
    return 0; 
  }
}

inline t_sint16 aux_hamac_pipe_get_read() {
 return hamac_pipe->aux_hamac_pipe_read_pos;
}

inline void aux_hamac_pipe_set_read(t_sint16 pipe_read) {
 hamac_pipe->aux_hamac_pipe_read_pos = pipe_read;
}

