/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


// This file is aimed to be included in the hpp file of the codec,
// in order to have the hamac_pipe global local to the class
public:
	t_hamac_pipe *hamac_pipe;

inline void hamac_pipe_init(t_hamac_pipe *hamac_pipe_ptr) {
	hamac_pipe = hamac_pipe_ptr;
	hamac_pipe->hamac_pipe_read=0;
	hamac_pipe->hamac_pipe_write=0; // we start by incrementing it in reset
	hamac_pipe->decoder_state=DECODER_STATE_NONE;
	hamac_pipe->pipe_is_empty = 1;
}

inline t_sint32 hamac_pipe_is_full() {
#ifdef __H264DEC__
  /* For H264DEC, void frames have been conveyed through the Auxiliary pipe, so now we
  want to exploit the Hamac pipe to its full extent, this is why the "pipe full" condition
  has been modified to W=R instead of W=R-1. But in order to change this condition
  we need to properly handle the "pipe empty" condition */
	if((hamac_pipe->hamac_pipe_read == hamac_pipe->hamac_pipe_write)&&(!hamac_pipe->pipe_is_empty))
    return 1;
#else	
  /* Note that with this implementation, FREE_SLOTS must be strictly > 0,
     i.e. with this implementation there is always at least one unused 
     slot in the pipe (makes sense only if the pipe conveys void frames) */
	t_sint16 delta;
	delta = hamac_pipe->hamac_pipe_read - hamac_pipe->hamac_pipe_write-1; 
	if (delta<0) delta+=HAMAC_PIPE_SIZE+1;
	if (delta<HAMAC_PIPE_FREE_SLOTS) return 1;
#endif  
	return 0;
}

inline void hamac_pipe_next_read_pos() {
	// increment location where hamac_pipe data are read
	t_sint16 hamac_pipe_read;
	hamac_pipe_read = hamac_pipe->hamac_pipe_read+1;
	if (hamac_pipe_read == HAMAC_PIPE_SIZE+1) hamac_pipe_read=0;
	hamac_pipe->hamac_pipe_read_ptr = &hamac_pipe->hamac_pipe_info[hamac_pipe_read];
	#ifdef VERBOSE_HAMAC_PIPE
	NMF_LOG("hamac_pipe_next_read_pos %d\n", hamac_pipe_read);
	#endif
}

inline void hamac_pipe_next_read() {
	// unlock hamac_pipe location
	hamac_pipe->hamac_pipe_read++;
	if (hamac_pipe->hamac_pipe_read == HAMAC_PIPE_SIZE+1) hamac_pipe->hamac_pipe_read=0;
	if(hamac_pipe->hamac_pipe_write==hamac_pipe->hamac_pipe_read)hamac_pipe->pipe_is_empty=1;
	#ifdef VERBOSE_HAMAC_PIPE
	NMF_LOG("hamac_pipe_next_read %d\n", hamac_pipe->hamac_pipe_read);
	#endif
}

inline void hamac_pipe_next_write() {
	hamac_pipe->hamac_pipe_write++;
	hamac_pipe->pipe_is_empty = 0;
	if (hamac_pipe->hamac_pipe_write == HAMAC_PIPE_SIZE+1) hamac_pipe->hamac_pipe_write=0;
	hamac_pipe->hamac_pipe_write_ptr = &hamac_pipe->hamac_pipe_info[hamac_pipe->hamac_pipe_write];
#ifndef __H264DEC__
  /* this condition is not compatible anymore with H264dec because we exploit the full hamac pipe */
	if ((HAMAC_PIPE_SIZE>0) && 
		(hamac_pipe->hamac_pipe_write == hamac_pipe->hamac_pipe_read)) {
		#ifdef VERBOSE_HAMAC_PIPE
		NMF_LOG ("\nERROR: HAMAC_PIPE write full. %d\n", hamac_pipe->hamac_pipe_write);
		#endif
		NMF_PANIC("\nERROR: HAMAC_PIPE write full.\n");
	}
#endif	
}

inline void hamac_pipe_rewind_write() {
	hamac_pipe->hamac_pipe_write--;
	if (hamac_pipe->hamac_pipe_write == -1) hamac_pipe->hamac_pipe_write=HAMAC_PIPE_SIZE;
	if(hamac_pipe->hamac_pipe_write==hamac_pipe->hamac_pipe_read)hamac_pipe->pipe_is_empty=1;
	#ifdef VERBOSE_HAMAC_PIPE
    NMF_LOG("Rewind pipe at w%d r%d\n", hamac_pipe->hamac_pipe_write, hamac_pipe->hamac_pipe_read);
	#endif
}

inline void hamac_pipe_reset_frame() {
	hamac_pipe_next_write();
	hamac_pipe->hamac_pipe_write_ptr->bumped_frames_number = 0;
	hamac_pipe->hamac_pipe_write_ptr->void_frame_info = VOID_FRAME_NONE;
	#ifdef VERBOSE_HAMAC_PIPE
	NMF_LOG("Reset pipe at w%d r%d\n", hamac_pipe->hamac_pipe_write, hamac_pipe->hamac_pipe_read);
	#endif
}

inline void hamac_pipe_add_bumped_frame(t_frames *frame) {
	hamac_pipe->hamac_pipe_write_ptr->bumped_frames[
		hamac_pipe->hamac_pipe_write_ptr->bumped_frames_number].bumped_frame = frame;
	hamac_pipe->hamac_pipe_write_ptr->bumped_frames_number++;
	#ifdef VERBOSE_HAMAC_PIPE
	NMF_LOG("\tAdding frame %p in pipe %d\n", frame, hamac_pipe->hamac_pipe_write);
	#endif
}

inline t_sint16 hamac_pipe_mark_eos_frame() {
	if (hamac_pipe->hamac_pipe_write_ptr->bumped_frames_number)
	{
		hamac_pipe->hamac_pipe_write_ptr->bumped_frames[
			hamac_pipe->hamac_pipe_write_ptr->bumped_frames_number-1].bumped_frame->nFlags 
				|= OSI_OMX_BUFFERFLAG_EOS;
		return 1;
 	}
	return 0;
}

inline t_uint32 hamac_pipe_bumped_frames_number() {
	return hamac_pipe->hamac_pipe_write_ptr->bumped_frames_number;
}

inline t_hamac_param *hamac_pipe_get_write_param_in() {
	return &hamac_pipe->hamac_pipe_write_ptr->param_in;
}

inline t_hamac_param *hamac_pipe_get_read_param_in(){
	return &hamac_pipe->hamac_pipe_read_ptr->param_in;
}

inline void hamac_pipe_set_bit_buffer_addr(t_bit_buffer *bit_buffer_addr) {
	hamac_pipe->hamac_pipe_write_ptr->bit_buffer_addr =bit_buffer_addr ;
}

inline t_bit_buffer *hamac_pipe_get_bit_buffer_addr() {
	if (!hamac_pipe || !hamac_pipe->hamac_pipe_read_ptr) {
		return 0;
	}
	return hamac_pipe->hamac_pipe_read_ptr->bit_buffer_addr;
}

inline void hamac_pipe_set_void_frame_info(t_void_frame_info void_frame_info) {
	hamac_pipe->hamac_pipe_write_ptr->void_frame_info = void_frame_info;
}

inline t_void_frame_info hamac_pipe_get_void_frame_info() {
	return hamac_pipe->hamac_pipe_read_ptr->void_frame_info;
}

inline void hamac_pipe_set_frame_pos(t_sint16 frame_pos) {
	hamac_pipe->hamac_pipe_write_ptr->frame_pos = frame_pos;
}

inline t_sint16 hamac_pipe_get_frame_pos() {
	return hamac_pipe->hamac_pipe_read_ptr->frame_pos;
}

inline void set_decoder_state(t_decoder_state decoder_state) {
	if (hamac_pipe->decoder_state == DECODER_STATE_NONE) { 
		hamac_pipe->decoder_state = decoder_state;
	}
}

inline t_decoder_state get_decoder_state() {
	return hamac_pipe->decoder_state;
}





//
// Auxiliary hamac pipe used to convey void frames, it is just a basic fifo 
// that you can push, pop and read from
//
//
//


/********************************************************************************
 * Push a new element into aux hamac pipe fifo, this will move to next position
 * return 0 if the FIFO is full (this is an error situation that must never happen)
 * return 1 in normal case 
 ********************************************************************************/ 
inline t_uint16 aux_hamac_pipe_fifo_push(t_void_frame_info void_frame, t_uint32 index)
{
#ifdef VERBOSE_HAMAC_PIPE
    NMF_LOG("PUSH AUX %d at %d\n",void_frame,hamac_pipe->aux_hamac_pipe_write_pos);
#endif
	t_uint16 new_pos = hamac_pipe->aux_hamac_pipe_write_pos + 1;
	if (new_pos >= AUX_HAMAC_PIPE_SIZE) new_pos=0;

	if (new_pos == hamac_pipe->aux_hamac_pipe_read_pos) 
  {
#ifdef VERBOSE_HAMAC_PIPE
    NMF_LOG("AUX FIFO FULL\n");
#endif
    return 0; // FIFO full
  }
	hamac_pipe->aux_hamac_pipe_info[hamac_pipe->aux_hamac_pipe_write_pos].void_frame_info = void_frame;
	hamac_pipe->aux_hamac_pipe_info[hamac_pipe->aux_hamac_pipe_write_pos].index = index;
	hamac_pipe->aux_hamac_pipe_write_pos = new_pos;	
	return 1;
}

/**********************************************************************
 * Reset informations for next element, to be called before push
 **********************************************************************/ 
inline void aux_hamac_pipe_fifo_reset_frame()
{
	hamac_pipe->aux_hamac_pipe_info[hamac_pipe->aux_hamac_pipe_write_pos].bumped_frames_number = 0;  
}

/**************************************************************************************
 * Copy infos on frames to be bumped from current position of standard hamac pipe FIFO
 * starting from offset  
 **************************************************************************************/ 
inline void aux_hamac_pipe_fifo_copy_bumped_frames(t_uint32 offset)
{
  t_uint16 i;
  t_uint16 nb = hamac_pipe->hamac_pipe_write_ptr->bumped_frames_number;

#ifdef VERBOSE_HAMAC_PIPE
    NMF_LOG("COPY BUMP AUX %d at %d, offset %d\n",nb,hamac_pipe->aux_hamac_pipe_write_pos, offset);
#endif

  hamac_pipe->aux_hamac_pipe_info[hamac_pipe->aux_hamac_pipe_write_pos].bumped_frames_number 
    = hamac_pipe->hamac_pipe_write_ptr->bumped_frames_number - offset;
  for(i=offset;i<nb;i++)
  {
    hamac_pipe->aux_hamac_pipe_info[hamac_pipe->aux_hamac_pipe_write_pos].bumped_frames[i-offset].bumped_frame
      = hamac_pipe->hamac_pipe_write_ptr->bumped_frames[i].bumped_frame; 
  }
  hamac_pipe->hamac_pipe_write_ptr->bumped_frames_number = offset;
}

/*****************************************************
 * pop an element from fifo, move to next position
 *****************************************************/ 
inline void aux_hamac_pipe_fifo_pop() {
#ifdef VERBOSE_HAMAC_PIPE
    NMF_LOG("POP AUX\n");
#endif
	hamac_pipe->aux_hamac_pipe_read_pos++;
	if (hamac_pipe->aux_hamac_pipe_read_pos >= AUX_HAMAC_PIPE_SIZE) hamac_pipe->aux_hamac_pipe_read_pos=0;
}

/*****************************************************
 * initialize read and write pointers for the fifo
 *****************************************************/ 
inline void aux_hamac_pipe_fifo_init() {
	hamac_pipe->aux_hamac_pipe_read_pos = hamac_pipe->aux_hamac_pipe_write_pos = 0;
}

/********************************************************************
 * get current element from FIFO without pop (no move to next pos !)
 ********************************************************************/ 
inline t_aux_hamac_pipe_info * aux_hamac_pipe_fifo_get_current_el() {
	return &hamac_pipe->aux_hamac_pipe_info[hamac_pipe->aux_hamac_pipe_read_pos];
}


