/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __parser_cpp
#define __parser_cpp
#include "h264dec/arm_nmf/inc/settings.h"
#include "h264dec/arm_nmf/inc/types.h"
#include "h264dec/arm_nmf/inc/host_types.h"
#include "h264dec/arm_nmf/inc/hamac_types.h"
#include "h264dec/arm_nmf/inc/host_decoder.h"
#include "h264dec/arm_nmf/inc/host_sei.h"
#include "FSM.h"
#include "parserEvent.h"

#ifdef __ndk5500_a0__
#include "inc/type.h"
#define ARMNMF_DBC_ASSERT_MSG(a, msg) { if (!(a)) { NMF_PANIC("%s - %s:%d\n", msg, __FILE__, __LINE__) ; }}

#define ARMNMF_DBC_ASSERT(a)           ARMNMF_DBC_ASSERT_MSG(a, "ASSERTION FAILED")
#define ARMNMF_DBC_PRECONDITION(a)     ARMNMF_DBC_ASSERT_MSG(a, "PRECONDITION FAILED")
#define ARMNMF_DBC_POSTCONDITION(a)    ARMNMF_DBC_ASSERT_MSG(a, "POSTCONDITION FAILED")
#define ARMNMF_DBC_INVARIANT(a)        ARMNMF_DBC_ASSERT_MSG(a, "INVARIANT FAILED")
#else
#include "armnmf_dbc.h"
#endif


class h264dec_arm_nmf_parser:public FSM , public h264dec_arm_nmf_parserTemplate
{
    /* globals */
    
    /* methods private */
    
    /*constructor*/
public:
  //  h264dec_arm_nmf_decoder();
    
    /* methods itf */
public:
//virtual t_nmf_error  construct(void);
virtual void start(void);
virtual void stop(void);
//virtual void destroy(void);

// Interfaces
virtual void 	nal(t_bit_buffer *p_b,void *sei_val, t_uint32 nTimeStampH, t_uint32 nTimeStampL, t_uint32 nFlags);
virtual void 	input_decoder_command(t_command_type command_type);
virtual void    set_parser_parameter(t_uint16 ack);

virtual void initTraceInfo(TraceInfo_t * ptr, unsigned int val);

virtual	void nal_handshake(t_handshake mode,t_bit_buffer *p_b);
virtual void fatal_decoding_error(t_uint16 error_code);

void 	nal_old(t_bit_buffer *p_b,void *sei_val, t_uint32 nTimeStampH, t_uint32 nTimeStampL, t_uint32 nFlags);
void nal_handshake_old(t_handshake mode,t_bit_buffer *p_b);


// Internal function 
void	nal_process(t_dec *dec,t_bit_buffer *p_b,t_SEI *sei,t_uint16 parser_status, t_uint32 nTimeStampH, t_uint32 nTimeStampL, t_uint32 nFlags);
void CreateNalList(t_bit_buffer *p_b);
//	public:


private:
void	InitialState (const ParserEvent *evt);
void	FlushState (const ParserEvent *evt);
void 	WaitRequestedFrame	(const ParserEvent *evt) ;
void 	WaitAnticipatedFrame	(const ParserEvent *evt) ;
void 	FrameReady(const ParserEvent *evt) ;

// One  input is under processing in decoder
// Some buffers are in nal input fifo
// one buffer is under processing in decoder
// if fifo size is change in parser.conf , please change this define !!
#define NAL_FIFO_SIZE	1
//#define NAL_BOUNDARY_ARRAY_COUNT	1+NAL_FIFO_SIZE+1 
#define NAL_BOUNDARY_ARRAY_COUNT	2 
t_nal_boundaries*	GetAvailableBoundaryArray(void);
void InitNalBoundaryArray(void);
t_nal_boundaries	NalBoundariesArray[NAL_BOUNDARY_ARRAY_COUNT];
t_uint32			NalBoundariesIdx;

public:
t_uint32	PendingDecoderRequest;
t_uint32	PendingParserRequest;
typedef struct dd
{
	t_uint32		Ready;
	t_bit_buffer 	*p_b;
	t_SEI 			*sei_val;
	t_uint32 		nTimeStampH;
	t_uint32 		nTimeStampL;
	t_uint32 		nFlags;
}
t_DataBuffer;


t_DataBuffer		DataBuffer;
};



#endif

