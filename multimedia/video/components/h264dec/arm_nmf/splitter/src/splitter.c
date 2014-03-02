/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */



/*
 * Include
 */
#ifdef NMF_BUILD
#include <h264dec/arm_nmf/splitter.nmf>
#endif
#include <types.h>
#include <host_sei.h>
#include <host_bitstream.h>


#ifdef NMF_BUILD
#define NMF_INTERFACE(a,b)      a.b
#else
void	nal(t_bit_buffer *p_b,void *sei_val,t_uint16 decstop, t_uint32 nTimeStampH, t_uint32 nTimeStampL, t_uint32 nFlags, void *ctxt);
#define NMF_INTERFACE(a,b)      b
#endif

/***************************************************************************/
/*
 * Provide name : input
 * Interface    : api.add
 * Param        : a 
 * Param        : b 
 *
 * Add params a and b
 * Provide result to the output interface
 */
/***************************************************************************/

void	METH(nal)(t_bit_buffer *p_b,t_uint32 bs_end,void *sei_val,t_uint16 parser_status, t_uint32 nTimeStampH, t_uint32 nTimeStampL, t_uint32 nFlags, void *ctxt)
{
	t_uint16 		parsestatus;
		t_uint32 		next;

	/* Start bitstream parsing */
	do
	{
		t_uint16 		size;
		t_bit_buffer	p_buf;

		// Update the p_b->addr field ans skip start code
		parsestatus = FindNextNALU(p_b,&size,&next);

		// No error so sned the NAL
		if (parsestatus!=1) NMF_INTERFACE(output,nal)(p_b,(t_uint32) next,sei_val,0,nTimeStampH,nTimeStampL,nFlags,ctxt);

	} 
	while (parsestatus == 0);
	// Repeat LAST NAL if EOS
	if (parser_status==2)
	{
		NMF_INTERFACE(output,nal)(p_b,(t_uint32) next,sei_val,parser_status,nTimeStampH,nTimeStampL,nFlags,ctxt);
	}

}


 
