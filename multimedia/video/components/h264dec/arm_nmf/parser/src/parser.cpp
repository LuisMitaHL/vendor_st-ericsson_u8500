/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#include <h264dec/arm_nmf/parser.nmf>
#include "ost_event.h"

extern "C" {
//FIXME: this can be removed once NMF correctly exports this function
#if ((!defined __SYMBIAN32__) && (!defined IMPORT_C))
    #define IMPORT_C
#endif
#if ((!defined __SYMBIAN32__) && (!defined EXPORT_C))
    #define EXPORT_C
#endif

extern IMPORT_C void NMF_PANIC(const char* fmt, ...);

}

#include "lifecyclecheck.h"
//#define DEBUG_PARSER
void METH(start)(void)
{
	set_parser_parameter(0);
}

void METH(stop)(void)
{
}

void METH(initTraceInfo)(TraceInfo_t * ptr, unsigned int val)
{
  decoder_trace_init_out.initTraceInfo(ptr,val);
}

void METH(set_parser_parameter)(t_uint16 ack)
{
	NalBoundariesIdx=0;
	//Initialize state machine
    state = 0;  // variable of FSM
	FSM::init((FSM_State)&METH(InitialState));
    
    if (ack) {
        pendingCommandAck.pendingCommandAck(COMMAND_COMPLETED);
    }
}


#ifdef BIG_ENDIAN
#define FIRST_BYTE(a)	((a)>>8)
#define LAST_BYTE(a)	((a) &0xff)
#define CODE_0x0000			0
#define CODE_0x0001			0x0001
#define H264_PARSER_MASK_BIT			~0x0001
#else
#define FIRST_BYTE(a)	((a) & 0xff)
#define LAST_BYTE(a)	((a) >> 8)
#define CODE_0x0000			0
#define CODE_0x0001			0x0100
#define H264_PARSER_MASK_BIT			~0x0100
#endif


t_uint8 *search_start_code(t_uint8 *p_tmp,t_uint8 *p_end)
{
	t_uint32	i,n,found=0;

	t_uint16	code;
	t_uint32	lastbyte_is_one=0;
	
	// aligning start pointer on even address
	if ((t_uint32) p_tmp & 1)
	{
	 if ( (*p_tmp==0x00) && (*(p_tmp+1)==0x00) &&  (*(p_tmp+2)==0x01) ) 
     {
	  return p_tmp;
	 }
     p_tmp++;
	}

	// aligning end pointer on even address
	if (((t_uint32)p_end & 1 )== 1)
	{
	 p_end--;
	 if (*p_end==1) lastbyte_is_one=1;
	}


	// p_end point one location after last array element
	// p_end=4 so byte from 0,1,2,3
	// Then we iterate one one time , because first iteration is moved out of loop 
	// to avoid to read to inknow location
	t_uint16 	*p=(t_uint16*) p_tmp;

	/* MC: bug fix to avoid case of n=-1 */
	if(p_end==p_tmp)return(t_uint8*)0;
	
	n= (p_end - p_tmp)/2-1;

	code=*p++;

	// to avoid first false detection
    if (code==CODE_0x0001) code=0xffff;	
		
	// sync word is 0x 00 00 01  
	// we read data 16 bit per 16 bit 
	// On a sync word , we can fetch a data word equal to 0x0000 or to 0x0001
	// depending on the byte alignement. Only two cases to check among
	// 65536 caseq, very strong probabily to have test false.
	// If code is true then  synch word confirmation is check
	
	// code is optimized to have loop code size less than 64 bytes
	// This ensure best efficienty regarding cache access and power
	for(i=n;i>0;i--)
	{
			if ((code & H264_PARSER_MASK_BIT)==0) 
			{
				if ((code==CODE_0x0000) && (FIRST_BYTE(*p)==1)) 
				{
					found=1;
					//p_tmp=(t_uint8*)p;
					//p_tmp-=2;	
					break;
				}
				if ((code==CODE_0x0001) && (LAST_BYTE(*(p-2))==0))
				{
					found=2;	
					//p_tmp=(t_uint8*)p;
					//p_tmp-=3;	
					break;
				}
			}
			code=*p++;
	}


	if ((code==CODE_0x0001) && (LAST_BYTE(*(p-2))==0))
	{
			return (t_uint8*)p -3;
	}

	if (lastbyte_is_one && (code==CODE_0x0000))
	{
			return (t_uint8*)p -2;

	}

	if (found==1) 
	{
		p_tmp=(t_uint8*)p;
		p_tmp-=2;	
		return p_tmp;
	}

	if (found==2) 
	{
		p_tmp=(t_uint8*)p;
		p_tmp-=3;	
		return p_tmp;
	}

	return (t_uint8*)0;
}

#define VERBOSE_BITSTREAM 0


t_uint16 MyFindNextNALUfast(t_bit_buffer *p_b, t_uint16 *size, t_uint8 **p_next)
{
    t_uint8 *p_tmp, *p_start, *p_end;

  if (p_b->addr > p_b->end)
	{
		NMF_PANIC("Bad parameter for bitstream\n");
	}
 
	p_tmp=search_start_code(p_b->addr,p_b->end);

	// Start code not found so return EndOfBuffer
	if (p_tmp==0)  return 1;
   
    p_start = p_tmp; // MC: fixme: this was modified in High profile code, should we modify it here as well ??
    p_b->addr = p_tmp = p_tmp+3;
    p_b->os = 8;
#if VERBOSE_BITSTREAM > 0
    printf("\nNAL Unit found at byte offset 0x%lx\n",(t_uint32)(p_b->addr-p_b->start));
#endif

    /* Adjust start pointer to count zero_bytes */
    // MC: fixme: this was modified in High profile code, should we modify it here as well ??
    while((p_start >= p_b->start) && (*p_start == 0))
    {
        p_start--;
    }

	/* Determine the first byte of the next NALU */  
	p_tmp=search_start_code(p_b->addr,p_b->end);

	// Start code not found
    if (p_tmp==0)
    {
#if VERBOSE_BITSTREAM > 0
        printf("This is the last NAL unit in the bitstream\n");
#endif
      
        *p_next = p_b->end;
	    *size = (t_uint16)((t_uint32)p_b->end - (t_uint32)p_start);
        return 2;
    }
    else
    {
		/* Adjust start pointer to count zero_bytes */
		p_end=p_tmp;
		while((p_end >= p_b->start) && (*p_end == 0))
		{
			p_end--;
		} 	
		*p_next = p_tmp;
		*size = (t_uint16)((t_uint32)p_end - (t_uint32)p_start);
#if VERBOSE_BITSTREAM > 0
		printf("NAL unit size is %u\n",*size);
#endif
    }

    return 0;
}


extern "C" {
void	arch_arm_v7_memory_cache_l1_cleanAll();
}

void METH(CreateNalList)(t_bit_buffer *p_b)
{
		t_uint32	nal_count=0;
		t_uint32	parsestatus;
		t_uint8			*next;
		t_uint8			*addr;
		t_nal_boundaries *nal_b=(t_nal_boundaries*) p_b->nal_boundaries;

		addr=p_b->addr;
		//MeasureEventStart0(TRACE_EVENT_PARSER_START,"parser nal_handshake in");
		
		do
		{
			t_uint16 size;
			parsestatus = MyFindNextNALUfast(p_b,&size,&next);
			// no error
			if (parsestatus!=1)
			{
				nal_b->nal_start[nal_count]=p_b->addr;
				p_b->addr=next;
				nal_count++;	
			}
			// one more to know the end of last complete nal
			nal_b->nal_start[nal_count]=next;
			nal_b->nal_count=nal_count;

			if (nal_count>=MAX_NAL) NMF_PANIC("Too much Nals\n");

		}
		while (parsestatus == 0);
		//MeasureEventStop(TRACE_EVENT_PARSER_STOP,"h264 parser nal out");
		p_b->addr=addr;

}

void METH(InitNalBoundaryArray)(void)
{
	t_uint32 i;

	for(i=0;i<NAL_BOUNDARY_ARRAY_COUNT;i++)
	{
		NalBoundariesArray[i].nal_count=0;
	}
}



t_nal_boundaries*	METH(GetAvailableBoundaryArray)(void)
{
	t_nal_boundaries*	 p=&NalBoundariesArray[NalBoundariesIdx++];
	if (p->nal_count!=0)
	{
		NMF_PANIC("This Nal array is still in use \n");
	}
	if (NalBoundariesIdx >= NAL_BOUNDARY_ARRAY_COUNT) NalBoundariesIdx=0;
	return p;
}

void	METH(nal)(t_bit_buffer *p_b,void *sei_val, t_uint32 nTimeStampH, t_uint32 nTimeStampL, t_uint32 nFlags)
{
	ParserEvent ev;

	
	ev.fsmEvent.signal = (FSM_FSMSIGnal)PARSER_GET_NAL;

	ev.args.nal.p_b=p_b;
	ev.args.nal.sei_val=sei_val;
	ev.args.nal.nTimeStampH=nTimeStampH;
	ev.args.nal.nTimeStampL=nTimeStampL;
	ev.args.nal.nFlags=nFlags;

	dispatch((FsmEvent *) &ev);

}

void METH(nal_handshake)(t_handshake mode,t_bit_buffer *p_b)
{
	ParserEvent ev;
	ev.fsmEvent.signal = (FSM_FSMSIGnal)PARSER_GET_DECODER_REQUEST;

	ev.args.req.mode=mode;
	ev.args.req.p_b=p_b;
//	NMF_LOG("Parser dispatch %d %p\n",mode,p_b);
	dispatch((FsmEvent *) &ev);
}
	




void METH(input_decoder_command)(t_command_type command_type) {
    if (command_type == COMMAND_FLUSH) {
    	ParserEvent ev;
    	ev.fsmEvent.signal = (FSM_FSMSIGnal)PARSER_FLUSH_REQUEST;
    	dispatch((FsmEvent *) &ev);
    } else {
    	output.input_decoder_command(command_type);
    }
}



// Here we enter in the initial state after initialization
// The parser wait for a request of the Decoder
void METH(InitialState)(const ParserEvent *evt) {
    switch(ParserEvent_FSMSIGnal(evt)) {
        case FSM_ENTRY_FSMSIG: 
					InitNalBoundaryArray();
           break;

		case FSM_EXIT_FSMSIG:
            break;

		case PARSER_GET_DECODER_REQUEST:
			// transmit request to Proxy and then move Wait state
			// 
		    //PRE_CONDITION(evt->args.req.p_b == 0);
		    //PRE_CONDITION(evt->args.req.mode == XON);
			if (evt->args.req.mode==XON)
			{
				nal_ctl_out.nal_handshake(XON,evt->args.req.p_b);
				this->state = (FSM_State)&METH(WaitRequestedFrame);
			}
			else
			{
				nal_ctl_out.nal_handshake(evt->args.req.mode,evt->args.req.p_b);
			}
			break;
   
        case PARSER_FLUSH_REQUEST:
        	output.input_decoder_command(COMMAND_FLUSH);
			this->state = (FSM_State)&METH(FlushState);
            break;

        default:
            ARMNMF_DBC_ASSERT(0);
            break;
    }
} 

void METH(WaitRequestedFrame)(const ParserEvent *evt) {
    switch(ParserEvent_FSMSIGnal(evt)) {
        case FSM_ENTRY_FSMSIG:  
           break;

		case FSM_EXIT_FSMSIG:
            break;
	
		case PARSER_GET_NAL:
			if (evt->args.nal.p_b)	
			{
				t_bit_buffer *p_b=evt->args.nal.p_b;
				p_b->nal_boundaries=(void*)	GetAvailableBoundaryArray();
				CreateNalList(p_b);
			}
			output.nal(evt->args.nal.p_b,evt->args.nal.sei_val,evt->args.nal.nTimeStampH,evt->args.nal.nTimeStampL,evt->args.nal.nFlags);

			// Here anticipating future request
			if (evt->args.nal.p_b)
			{
				nal_ctl_out.nal_handshake(XON,0);
				this->state = (FSM_State)&METH(WaitAnticipatedFrame);
			}
			else 
			// p_b was null , so it was the last input buffer
			// so we move to init state ?
			{
				this->state = (FSM_State)&METH(InitialState);
			}
			break;


		case PARSER_GET_DECODER_REQUEST:
			// Decoder can only free buffer not require more data
			//NMF_LOG("Wait get dec XOFF %p\n",evt->args.req.p_b);
		    PRE_CONDITION((evt->args.req.p_b != 0) || (evt->args.req.mode == PAUSE_ACK));
		    PRE_CONDITION(evt->args.req.mode != XON);
			nal_ctl_out.nal_handshake(evt->args.req.mode,evt->args.req.p_b);
            break;
   
        case PARSER_FLUSH_REQUEST:
        	output.input_decoder_command(COMMAND_FLUSH);
			this->state = (FSM_State)&METH(FlushState);
            break;

        default:
            ARMNMF_DBC_ASSERT(0);
            break;
    }
} 


void METH(WaitAnticipatedFrame)(const ParserEvent *evt) {
    switch(ParserEvent_FSMSIGnal(evt)) {
        case FSM_ENTRY_FSMSIG:  
           break;

		case FSM_EXIT_FSMSIG:
            break;
	
		case PARSER_GET_NAL:
			if (evt->args.nal.p_b)	
			{
				t_bit_buffer *p_b=evt->args.nal.p_b;
				p_b->nal_boundaries=(void*)	GetAvailableBoundaryArray();
				CreateNalList(p_b);
			}
		    // memory the ready frame
			DataBuffer.p_b=evt->args.nal.p_b;
			DataBuffer.sei_val=(t_SEI*)evt->args.nal.sei_val;
			DataBuffer.nTimeStampH=evt->args.nal.nTimeStampH;
			DataBuffer.nTimeStampL=evt->args.nal.nTimeStampL;
			DataBuffer.nFlags=evt->args.nal.nFlags;
	
			this->state = (FSM_State)&METH(FrameReady);
			break;


		case PARSER_GET_DECODER_REQUEST:
		    if (evt->args.req.mode != XON)
			{
				nal_ctl_out.nal_handshake(evt->args.req.mode,evt->args.req.p_b);
			}
			else
			{
				//send back p_b if not null , but don't request more data
				if (evt->args.req.p_b) nal_ctl_out.nal_handshake(XOFF,evt->args.req.p_b);
				this->state = (FSM_State)&METH(WaitRequestedFrame);

			}
			break;
   
        case PARSER_FLUSH_REQUEST:
        	output.input_decoder_command(COMMAND_FLUSH);
			this->state = (FSM_State)&METH(FlushState);
            break;

                    
        default:
            ARMNMF_DBC_ASSERT(0);
            break;
    }
} 

//
// A frame is ready , so waiting for a decoder request
//
void METH(FrameReady)(const ParserEvent *evt) {
    switch(ParserEvent_FSMSIGnal(evt)) {
        case FSM_ENTRY_FSMSIG:  
           break;

		case FSM_EXIT_FSMSIG:
            break;
	
		case PARSER_GET_DECODER_REQUEST:
		    if (evt->args.req.mode != XON)
			{
				nal_ctl_out.nal_handshake(evt->args.req.mode,evt->args.req.p_b);
			}
			else
			{
				// send buffer 
				output.nal(DataBuffer.p_b,DataBuffer.sei_val,DataBuffer.nTimeStampH,DataBuffer.nTimeStampL,DataBuffer.nFlags);
				// send XON as an anticipated one
				if (DataBuffer.p_b)
				{
					nal_ctl_out.nal_handshake(evt->args.req.mode,evt->args.req.p_b);
					this->state = (FSM_State)&METH(WaitAnticipatedFrame);
				}
				else
				{
					// it was the last input buffer so we don't request more buffer
					// but we simply free buffer if needed 
					if (evt->args.req.p_b) nal_ctl_out.nal_handshake(XOFF,evt->args.req.p_b);
					this->state = (FSM_State)&METH(InitialState);
				}
			}
			break;
   
        case PARSER_FLUSH_REQUEST:
            // sends the nal that was anticipated
        	output.input_decoder_command(COMMAND_FLUSH);
    		if (DataBuffer.p_b)
			{
				nal_ctl_out.nal_handshake(XOFF, DataBuffer.p_b);
                DataBuffer.p_b = 0;
			}
  			this->state = (FSM_State)&METH(FlushState);
            break;
                    
        default:
            ARMNMF_DBC_ASSERT(0);
            break;
    }
} 

void METH(FlushState)(const ParserEvent *evt) {
    switch(ParserEvent_FSMSIGnal(evt)) {
        case FSM_ENTRY_FSMSIG:  
           break;

		case FSM_EXIT_FSMSIG:
            break;
	
		case PARSER_GET_DECODER_REQUEST:
    		nal_ctl_out.nal_handshake(evt->args.req.mode, evt->args.req.p_b);
            if (evt->args.req.mode == FLUSH_ACK) {
    			this->state = (FSM_State)&METH(InitialState);
            }
			break;
                    
        default:
            ARMNMF_DBC_ASSERT(0);
            break;
    }
} 

void METH(fatal_decoding_error)(t_uint16 error_code)
{
 nal_ctl_out.fatal_decoding_error(error_code);
}
