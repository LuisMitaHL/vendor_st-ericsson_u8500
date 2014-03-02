/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _PARSER_EVENT_H_
#define _PARSER_EVENT_H_
#include <FsmEvent.h>


typedef enum {
    PARSER_EVENT_FSMSIG=FSM_USER_FSMSIG,
    PARSER_GET_NAL,
    PARSER_GET_DECODER_REQUEST,
    PARSER_FLUSH_REQUEST
} ParserSignal;

typedef struct {
	FsmEvent    fsmEvent; 
	union 
	{
		struct {
			t_bit_buffer *p_b;
			void *sei_val;
			t_uint32 nTimeStampH;
			t_uint32 nTimeStampL;
			t_uint32 nFlags;
		}
		nal;
		struct {
				t_handshake mode;
				t_bit_buffer *p_b;
		}
		req;

	} args ;
} ParserEvent;

static inline int ParserEvent_FSMSIGnal(const ParserEvent *evt) { return evt->fsmEvent.signal; }
#endif // _PARSER_EVENT_H_
