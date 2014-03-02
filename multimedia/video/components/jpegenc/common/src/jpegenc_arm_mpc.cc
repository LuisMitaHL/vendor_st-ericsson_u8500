/******************************************************************************
 Copyright (c) 2009-2011, ST-Ericsson SA
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials
      provided with the distribution.
   3. Neither the name of the ST-Ericsson SA nor the names of its
      contributors may be used to endorse or promote products
      derived from this software without specific prior written
      permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************/

/*----------------------------------------------------------------------------
 |  Common Code for both ARM and MPC - this file is included inside MPC or   |
 |  ARM-NMF code separated by compilation flag.                              |
 -----------------------------------------------------------------------------*/



#ifdef __JPEGDENC_ARM_NMF
	#include <common/inc/jpegenc_arm_mpc.h> //common header file for both arm and mpc
	#include <jpegenc/arm_nmf/parser.nmf>
	#include <parser.hpp>
#ifdef __JPEGDENC_NDK_5500_
	#include "mmhwbuffer.h"
#endif

	//#ifndef ENABLE_ARMNMF_LOGS
	//#define  printf(...)
	//#endif

#else
	#include <jpegenc/mpc/ddep.nmf>
	#include <inc/archi-wrapper.h>
	#include <fsm/component/include/Component.inl>
	#include <vfm_vec_jpeg.idt>
	#include <jpegenc_arm_mpc.h> //common header file for both arm and mpc
	#include <ddep.h>
#endif


#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#ifdef __JPEGDENC_ARM_NMF
	#include "video_components_jpegenc_arm_nmf_parser_src_parserTraces.h"
#else
	#include "video_components_jpegenc_mpc_ddep_src_ddepTraces.h"
#endif
#endif //for OST_TRACE_COMPILER_IN_USE

#ifndef __JPEGDENC_ARM_NMF
// Global allocation for all trace ressources
//<>static TraceInfo_t EXTMEM traceInfo   = {0,0,0};
//<>static TRACE_t     EXTMEM traceObject = {&traceInfo, 0};
static TRACE_t *   EXTMEM this;//<> = (TRACE_t *)&traceObject;
#endif
#ifdef __JPEGDENC_ARM_NMF
	#define BUFFERFLAG_EOS OMX_BUFFERFLAG_EOS
	#define BUFFERFLAG_ENDOFRAME OMX_BUFFERFLAG_ENDOFFRAME
	#define BUFFERFLAG_DATACORRUPT OMX_BUFFERFLAG_DATACORRUPT
#endif

#ifdef __JPEGDENC_ARM_NMF
STATIC_FLAG void COMP(ReleaseBuffer)(t_uint32 port_idx,OMX_BUFFERHEADERTYPE* buf)
#else
STATIC_FLAG void COMP(ReleaseBuffer)(t_uint32 port_idx,Buffer_p buf)
#endif
{
#ifdef 	__JPEGDENC_ARM_NMF
        if (port_idx == 0)
        {
			#ifdef ENABLE_ARMNMF_LOGS
				OstTraceFiltInst0(TRACE_DEBUG, "JPEGEnc Ddep:  Releasing the input buffer \n");
        		printf("Releasing the input buffer \n");
        	#endif
		}
        if (port_idx == 1)
        {
			#ifdef ENABLE_ARMNMF_LOGS
				OstTraceFiltInst0(TRACE_DEBUG, "JPEGEnc Ddep:  Releasing the output buffer \n");
        		printf("Releasing the output buffer \n");
        	#endif
		}

	if(isDispatching())
	{
		mPorts[port_idx].returnBuffer(buf);
	}
	else
	{
		#ifdef ENABLE_ARMNMF_LOGS
			OstTraceFiltInst0(TRACE_DEBUG, "JPEGEnc Ddep: return from Asyn - input \n");
			printf("return from Asyn - input \n");
		#endif
		returnBufferAsync(port_idx,buf);
    	}

	//returnBufferAsync(port_idx,buf);
#else
    OmxEvent ev;
	OstTraceFiltInst1(TRACE_DEBUG, "JPEGEnc Ddep: Entering ReleaseBuffer bufferport_index=%d\n",(t_uint32)port_idx);
	printf("\nentering ReleaseBuffer bufferport_index=%d",(t_uint32)port_idx);
    ev.fsmEvent.signal =  OMX_RETURNBUFFER_SIG;
    ev.args.returnBuffer.portIdx = port_idx;
    ev.args.returnBuffer.buffer = buf;
	FSM_dispatch(&mDataDep,&ev);
	OstTraceFiltInst0(TRACE_API, "JPEGEnc Ddep: exiting ReleaseBuffer\n");
	printf("\n exiting ReleaseBuffer");
#endif
}

#ifndef __JPEGDENC_ARM_NMF
t_uint16 buffer_available_atinput(Component *this)
{
	OstTraceFiltInst0(TRACE_API, "JPEGEnc Ddep : buffer_available_atinput enter \n");
	if(bufIn==0 && Port_queuedBufferCount(&mPorts[0]))
	{
		bufIn = Port_dequeueBuffer(&this->ports[0]);
		OstTraceFiltInst0(TRACE_FLOW, "JPEGEnc Ddep : New Input buffer available buf \n");
		return 1;
	}
	else if(bufIn)
		return 1;
	else
		return 0;
}
t_uint16 buffer_available_atoutput(Component *this)
{
	OstTraceFiltInst0(TRACE_API, "JPEGEnc Ddep : buffer_available_atoutput enter \n");
	if(bufOut_slice==0 && Port_queuedBufferCount(&mPorts[1]))
	{
		printf("\n  remove buffer output");
		bufOut_slice = Port_dequeueBuffer(&this->ports[1]);
		OstTraceFiltInst0(TRACE_FLOW, "JPEGEnc Ddep : New out buffer available buf \n");
		return 1;
	}
	else if(bufOut_slice)
		return 1;
	else
		return 0;

}
#endif


STATIC_FLAG void COMP(release_resources)(void)
{
#ifdef __JPEGDENC_ARM_NMF
	//nothing for ARM-NMF
#else
    // the HW resource must be released to avoid dead lock
	OstTraceFiltInst0(TRACE_API, "JPEGEnc Ddep: release_resources entering\n");
	printf("\n release_resources entering");
	ENTER_CRITICAL_SECTION;
    if (get_hw_resource)
	{
		 iResource.freeResource(RESOURCE_MMDSP, &iInformResourceStatus);
		 get_hw_resource=0;
	}
	else if(get_vpp_resource)
	{
		iResource.freeResource(RESOURCE_VPP, &iInformResourceStatus);
		get_vpp_resource=0;
	}
	EXIT_CRITICAL_SECTION;
	OstTraceFiltInst0(TRACE_API, "JPEGEnc Ddep: release_resources exiting \n");
	printf("\n release_resources exiting");
#endif
}

#ifdef __JPEGDENC_ARM_NMF
STATIC_FLAG void COMP(reset)()
#else
static void reset(struct Component *this)
#endif
{
	OstTraceFiltInst0(TRACE_API, "JPEGEnc Ddep: reset entering\n");
       	//printf("\nreset");
	release_resources();
}




STATIC_FLAG void COMP(linkin_loop)(void)
{
#ifdef __JPEGDENC_ARM_NMF
	//nothing to do for ARM-NMF
#else
	if(ps_ddep_bitstream_buf_link_and_header->s_ddep_bitstream_buf_link.addr_next_buf_link==NULL)
			{
			OstTraceFiltInst0(TRACE_FLOW, "JPEGEnc Ddep: linkin_loop() header \n");
		    printf("\n  header");
								ps_ddep_bitstream_buf_link_and_header->s_ddep_buffer_p = SVP_SHAREDMEM_TO16(0);
								ps_ddep_bitstream_buf_link_and_header->s_ddep_bitstream_buf_link.addr_next_buf_link = SVP_SHAREDMEM_TO16(s_jpege_ddep_desc.bbm_desc.nPhysicalAddress | EXT_BIT);
								ps_ddep_bitstream_buf_link_and_header->s_ddep_bitstream_buf_link.addr_buffer_start 	= SVP_SHAREDMEM_TO16(s_jpege_ddep_desc.bbm_desc.nPhysicalAddress + ADD_40);
								ps_ddep_bitstream_buf_link_and_header->s_ddep_bitstream_buf_link.addr_buffer_end 	= SVP_SHAREDMEM_TO16(s_jpege_ddep_desc.bbm_desc.nPhysicalAddress + ADD_100);

			}
			else
			{

			printf("\n  header1");
								ps_ddep_bitstream_buf_link_and_header1->s_ddep_buffer_p = SVP_SHAREDMEM_TO16(0);
								ps_ddep_bitstream_buf_link_and_header1->s_ddep_bitstream_buf_link.addr_next_buf_link = SVP_SHAREDMEM_TO16(s_jpege_ddep_desc.bbm_desc.nPhysicalAddress | EXT_BIT);
								ps_ddep_bitstream_buf_link_and_header1->s_ddep_bitstream_buf_link.addr_buffer_start 	= SVP_SHAREDMEM_TO16(s_jpege_ddep_desc.bbm_desc.nPhysicalAddress + ADD_80);
								ps_ddep_bitstream_buf_link_and_header1->s_ddep_bitstream_buf_link.addr_buffer_end 	= SVP_SHAREDMEM_TO16(s_jpege_ddep_desc.bbm_desc.nPhysicalAddress + ADD_100);

			}
			ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_out_bitstream_buffer.addr_bitstream_start = SVP_SHAREDMEM_TO16(s_jpege_ddep_desc.bbm_desc.nPhysicalAddress + ADD_40);
			codec_update_algo.updateAlgo(CMD_UPDATE_BUFFER);
			OstTraceFiltInst0(TRACE_API, "JPEGEnc Ddep: linkin_loop() exiting \n");
#endif
}


STATIC_FLAG void COMP(update_link)(void)
{
#ifdef __JPEGDENC_ARM_NMF
	//nothing to do for ARM-NMF
#else
	if(ps_ddep_bitstream_buf_link_and_header->s_ddep_bitstream_buf_link.addr_next_buf_link==NULL)
			{
				OstTraceFiltInst0(TRACE_FLOW, "JPEGEnc Ddep: update_link() header1 \n");
				printf("\n  header1");
				ps_ddep_bitstream_buf_link_and_header1->s_ddep_buffer_p = SVP_SHAREDMEM_TO16((t_uint32)bufOut_slice);
				ps_ddep_bitstream_buf_link_and_header->s_ddep_bitstream_buf_link.addr_next_buf_link = SVP_SHAREDMEM_TO16( s_jpege_ddep_desc.bbm_desc.nPhysicalAddress +  2*sizeof(ts_ddep_bitstream_buf_link_and_header) + 20  | EXT_BIT);/* use EXT_BIT here */
				ps_ddep_bitstream_buf_link_and_header1->s_ddep_bitstream_buf_link.addr_next_buf_link = SVP_SHAREDMEM_TO16(0);/* use EXT_BIT here */
				ps_ddep_bitstream_buf_link_and_header1->s_ddep_bitstream_buf_link.addr_prev_buf_link = SVP_SHAREDMEM_TO16(0);/* use EXT_BIT here */
				ps_ddep_bitstream_buf_link_and_header1->s_ddep_bitstream_buf_link.addr_buffer_start 	= SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(bufOut_slice->address));
				ps_ddep_bitstream_buf_link_and_header1->s_ddep_bitstream_buf_link.addr_buffer_end 	= SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(bufOut_slice->address) + 2*SVP_SHAREDMEM_FROM24(bufOut_slice->allocLen));

			}
			else
			{

				printf("\n  header");
				ps_ddep_bitstream_buf_link_and_header->s_ddep_buffer_p = SVP_SHAREDMEM_TO16((t_uint32)bufOut_slice);
				ps_ddep_bitstream_buf_link_and_header1->s_ddep_bitstream_buf_link.addr_next_buf_link = SVP_SHAREDMEM_TO16(s_jpege_ddep_desc.bbm_desc.nPhysicalAddress | EXT_BIT);/* use EXT_BIT here */
				ps_ddep_bitstream_buf_link_and_header->s_ddep_bitstream_buf_link.addr_next_buf_link = SVP_SHAREDMEM_TO16(0);/* use EXT_BIT here */
				ps_ddep_bitstream_buf_link_and_header->s_ddep_bitstream_buf_link.addr_prev_buf_link = SVP_SHAREDMEM_TO16(0);/* use EXT_BIT here */
				ps_ddep_bitstream_buf_link_and_header->s_ddep_bitstream_buf_link.addr_buffer_start 	= SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(bufOut_slice->address));
				ps_ddep_bitstream_buf_link_and_header->s_ddep_bitstream_buf_link.addr_buffer_end 	= SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(bufOut_slice->address) + 2*SVP_SHAREDMEM_FROM24(bufOut_slice->allocLen));

			}
			ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_out_bitstream_buffer.addr_bitstream_start = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(bufOut_slice->address));
		  	codec_update_algo.updateAlgo(CMD_UPDATE_BUFFER);
		  OstTraceFiltInst1(TRACE_FLOW, "JPEGEnc Ddep: update_link() ps_ddep_bitstream_buf_link_and_header->s_ddep_bitstream_buf_link.addr_next_buf_link : 0x%x \n",ps_ddep_bitstream_buf_link_and_header->s_ddep_bitstream_buf_link.addr_next_buf_link);
		  OstTraceFiltInst1(TRACE_FLOW, "JPEGEnc Ddep: update_link() ps_ddep_bitstream_buf_link_and_header1->s_ddep_bitstream_buf_link.addr_buffer_start : 0x%x \n",ps_ddep_bitstream_buf_link_and_header1->s_ddep_bitstream_buf_link.addr_buffer_start);
		  OstTraceFiltInst1(TRACE_FLOW, "JPEGEnc Ddep: update_link() ps_ddep_bitstream_buf_link_and_header1->s_ddep_bitstream_buf_link.addr_buffer_end : 0x%x \n",ps_ddep_bitstream_buf_link_and_header1->s_ddep_bitstream_buf_link.addr_buffer_end);
		  OstTraceFiltInst1(TRACE_FLOW, "JPEGEnc Ddep: update_link() ps_ddep_bitstream_buf_link_and_header1->s_ddep_buffer_p : 0x%x \n",ps_ddep_bitstream_buf_link_and_header1->s_ddep_buffer_p);
		  PRINT_VAR(ps_ddep_bitstream_buf_link_and_header->s_ddep_bitstream_buf_link.addr_next_buf_link);
		  PRINT_VAR(ps_ddep_bitstream_buf_link_and_header1->s_ddep_bitstream_buf_link.addr_buffer_start);
		  PRINT_VAR(ps_ddep_bitstream_buf_link_and_header1->s_ddep_bitstream_buf_link.addr_buffer_end);
		  PRINT_VAR(ps_ddep_bitstream_buf_link_and_header1->s_ddep_buffer_p);
		  OstTraceFiltInst0(TRACE_API, "JPEGEnc Ddep: update_link() exiting \n");
#endif

}


//#define endAlgo METH(endAlgo)
PUT_PRAGMA
#ifdef __JPEGDENC_ARM_NMF
void METH(endAlgo)(t_t1xhv_status status,t_t1xhv_encoder_info errors,t_uint32 durationInTicks,t_uint32 bitstream_size, t_uint32 ishvajpeg)
#else
void METH(endAlgo)(t_t1xhv_status status,t_t1xhv_encoder_info errors,t_uint32 durationInTicks,t_uint32 bitstream_size)
#endif
{
	OstTraceFiltInst2(TRACE_API, "JPEGEnc Ddep : Entering EndAlgo with status 0x%x and bitstreamSize : 0x%x \n",status,bitstream_size );

#ifdef __JPEGDENC_ARM_NMF
	#ifdef ENABLE_ARMNMF_LOGS
		printf("enter endAlgo\n");
	#endif
	//nothing to do for arm
#else
	printf("enter endAlgo\n");
    STORE_END_ALGO_TICKS(ps_ddep_sec_jpeg_param_desc_mpc_set/*, _till_ticks*/);
#endif
#ifdef __JPEGDENC_ARM_NMF

#ifdef __JPEGDENC_NDK_5500_

		if(ishvajpeg)
		{
		OMX_U8 *pheader;
		OMX_U32 header_size;
		OMX_U32 padd_offset, counter, padd_size;

		/*****************************************************************************************
		  *			~~~~~~~~~~~~~~~~~~~~~~~~~~~
		  * 			BELOW CODE IS WORK AROUD FOR HVA
		  *			~~~~~~~~~~~~~~~~~~~~~~~~~~~
		  *
		  * output buffer = header info + residual data
		  *
		  * HERE, output buffer is 64 byte aligned, therefore the pointer of header info is also aligned, but we
		  * cannot assure the pointer of residual data is also aligned, as it follows after header, so it depend on
		  * the size of header.
		  * In case of unaligned pointer (8 byte) for the bitstream buffer(residual data), in the output buffer,
		  * hva over-writes the header info in the output buffer, with the residual data values, because of which
		  * the header info at unaligned locations is over-written by some other data.

				Before giving control to HVA
			------------------------
		   Memory alignment of the bitstream, 8-byte
		  _________________________________________________________________________________
		 |00|01|02|03|04|05|06|07|08|00|01|02|03|04|05|06|07|08|00|01|02|03|04|05|06|07|08|
		 ``````````````````````````````````````````````````````````````````````````````````
			|										  |
		|                                                                                 |
		 _________________________________________________________________________________
		|H0|H1|H2|H3|H4 |  | |  |...|  |..............................................| | |
		 `````````````````````````````````````````````````````````````````````````````````

		Here,
		H0, H1, H2, H3, H4 -> are the header values
		Also, as shown above the header ends at unalign location i.e 5
		Bitsteam after giving control to HVA
		-----------------------------
		_________________________________________________________________________________
		|00|01|02|03|04|05|06|07|08|00|01|02|03|04|05|06|07|08|00|01|02|03|04|05|06|07|08|
		``````````````````````````````````````````````````````````````````````````````````
		| 								                 |
			|									         |
		_________________________________________________________________________________
		|C0|C1|C2|dd|ee|C0|C1|C2|C3|..|  |...............................................|
		``````````````````````````````````````````````````````````````````````````````````
		Here,
		C0, C1, C2, C3, C4, C5 -> Residual value of bitstream

		As in the above fiqure, the header values in the buffer is over written by HVA.
		So, re-write the no of values which are unaligned in the bitstream buffer with the header values.

		Here, as no of bytes unaligned is 5, so write the 5 values.

		**********************************************************************************/

		if (!bufOut_slice)
		{
			return;
		}
		// pheader is the start address of the header and header_size is the header size in bytes.
		pheader = (OMX_U8 *)((tps_t1xhv_vec_header_buf)param.addr_header_buffer)->addr_header_buffer;
		header_size = ((tps_t1xhv_vec_header_buf)param.addr_header_buffer)->header_size ;

		// check weather header size is aligned, if unaligned re-write the header values
		padd_size = header_size & 0x07 ;
		if(padd_size)
		{
		// calculate the offset for re-writing the headers
			   padd_offset = header_size - padd_size ;
		   //re-write the un-aligned location with the proper header values
		   for(counter = 0; counter < padd_size; counter++)
		   {
			*(unsigned char *)((unsigned int)(bufOut_slice->pBuffer) + padd_offset + counter) =
						 *(unsigned char *) ((unsigned int)(pheader) + padd_offset + counter);
		   }//end of  loop for(counter = 0; counter > padd_size; counter++)
		}
		}

#endif

#endif

    iSendLogEvent.eventEndAlgo(status,errors, durationInTicks,bitstream_size);

#ifdef __JPEGDENC_ARM_NMF
	#ifdef ENABLE_ARMNMF_LOGS
		printf("exit endAlgo\n");
	#endif
	//nothing to do for arm
#else
	printf("exit endAlgo\n");
#endif
	OstTraceFiltInst0(TRACE_API, "JPEGEnc Ddep: endAlgo exiting \n");
}



STATIC_FLAG void COMP(create_link)(ts_ddep_buffer_descriptor bbm_desc)
{
#ifdef __JPEGDENC_ARM_NMF
	//nothing todo for arm-nmf
#else
		OstTraceInt0(TRACE_API, "JPEGEnc Ddep: create_link entering \n");
		ps_ddep_bitstream_buf_link_and_header = (ts_ddep_bitstream_buf_link_and_header SHAREDMEM *)bbm_desc.nMpcAddress;
        //PRINT_VAR(ps_ddep_bitstream_buf_link_and_header);
		ps_ddep_bitstream_buf_link_and_header1 = (ts_ddep_bitstream_buf_link_and_header SHAREDMEM *) (bbm_desc.nMpcAddress + 0x20);
		//PRINT_VAR(ps_ddep_bitstream_buf_link_and_header1);
		OstTraceInt0(TRACE_API, "JPEGEnc Ddep: create_link exiting \n");
#endif
}


STATIC_FLAG void COMP(program_link)(ts_ddep_buffer_descriptor bbm_desc)
{
	OstTraceFiltInst0(TRACE_API, "JPEGEnc Ddep: program_link entering \n");
#ifdef 	__JPEGDENC_ARM_NMF
	//nothing todo for arm-nmf

    ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_frame_buffer.addr_source_buffer = (t_ahb_address)(bufIn->pBuffer) + (t_ahb_address)(bufIn->nOffset);
	ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_out_bitstream_buffer.addr_bitstream_start = (t_ahb_address)bufOut_slice->pBuffer + ((total_bitstream_size>>7)<<4); //new

#ifdef __JPEGDENC_NDK_5500_
	ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_out_bitstream_buffer.reserved_1 =  bufOut_slice->nAllocLen;
	OstTraceFiltInst1(TRACE_FLOW, "JPEGEnc Ddep: program_link() bufOut_slice->nAllocLen = %d \n",bufOut_slice->nAllocLen);
#endif

	ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_out_bitstream_buffer.bitstream_offset = (total_bitstream_size&0x7Ful); //new
//	ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_out_bitstream_buffer.addr_bitstream_buf_struct = (t_ahb_address)&bufOut_slice ;

{ //for Camera SAS v1.3
	ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_out_bitstream_buffer.addr_bitstream_buf_struct = (t_ahb_address)&ps_arm_nmf_jpeg_bitstream_buf_link;
	ps_arm_nmf_jpeg_bitstream_buf_link.addr_buffer_start = (t_ahb_address)bufOut_slice->pBuffer;
	ps_arm_nmf_jpeg_bitstream_buf_link.addr_buffer_end   = ps_arm_nmf_jpeg_bitstream_buf_link.addr_buffer_start + bufOut_slice->nAllocLen ;
}
	ps_ddep_sec_jpeg_param_desc_mpc_set->s_out_fram_buffer.addr_dest_buffer = (t_ahb_address)bufOut_slice->pBuffer ;
	ps_ddep_sec_jpeg_param_desc_mpc_set->s_header_buf.addr_header_buffer =  (t_ahb_address)ps_ddep_sec_jpeg_param_desc_mpc_set->header_buffer;


#else
	ps_ddep_bitstream_buf_link_and_header->s_ddep_buffer_p = SVP_SHAREDMEM_TO16((t_uint32)bufOut_slice);
	ps_ddep_bitstream_buf_link_and_header->s_ddep_bitstream_buf_link.addr_next_buf_link = SVP_SHAREDMEM_TO16(s_jpege_ddep_desc.bbm_desc.nPhysicalAddress | EXT_BIT);//making it cyclic for Camera SAS v1.3
	ps_ddep_bitstream_buf_link_and_header->s_ddep_bitstream_buf_link.addr_prev_buf_link = SVP_SHAREDMEM_TO16(0);/* use EXT_BIT here */
	ps_ddep_bitstream_buf_link_and_header->s_ddep_bitstream_buf_link.addr_buffer_start 	= SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(bufOut_slice->address));
	ps_ddep_bitstream_buf_link_and_header->s_ddep_bitstream_buf_link.addr_buffer_end 	= SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(bufOut_slice->address) + 2*SVP_SHAREDMEM_FROM24(bufOut_slice->allocLen));
	PRINT_VAR(SVP_SHAREDMEM_FROM24(bufOut_slice->address));
	PRINT_VAR(SVP_SHAREDMEM_FROM24(bufOut_slice->address) + 2*SVP_SHAREDMEM_FROM24(bufOut_slice->allocLen));
	ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_out_bitstream_buffer.addr_bitstream_buf_struct = SVP_SHAREDMEM_TO16(bbm_desc.nPhysicalAddress | EXT_BIT);
	ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_out_bitstream_buffer.addr_bitstream_start = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(bufOut_slice->address)+((total_bitstream_size>>7)<<4));
	ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_out_bitstream_buffer.bitstream_offset = SVP_SHAREDMEM_TO16(total_bitstream_size&0x7Ful);
	PRINT_VAR(SVP_SHAREDMEM_FROM24(bufOut_slice->address)+((total_bitstream_size>>7)<<4));
	PRINT_VAR(total_bitstream_size&0x7Ful);
	PRINT_VAR(total_bitstream_size);
	OstTraceFiltInst1(TRACE_FLOW, "JPEGEnc Ddep: program_link SVP_SHAREDMEM_FROM24(bufOut_slice->address) : 0x%x\n",SVP_SHAREDMEM_FROM24(bufOut_slice->address));
	OstTraceFiltInst1(TRACE_FLOW, "JPEGEnc Ddep: program_link SVP_SHAREDMEM_FROM24(bufOut_slice->address) + 2*SVP_SHAREDMEM_FROM24(bufOut_slice->allocLen): 0x%x\n",SVP_SHAREDMEM_FROM24(bufOut_slice->address) + 2*SVP_SHAREDMEM_FROM24(bufOut_slice->allocLen));
	OstTraceFiltInst1(TRACE_FLOW, "JPEGEnc Ddep: program_link SVP_SHAREDMEM_FROM24(bufOut_slice->address)+((total_bitstream_size>>7)<<4): 0x%x\n",SVP_SHAREDMEM_FROM24(bufOut_slice->address)+((total_bitstream_size>>7)<<4));
	OstTraceFiltInst1(TRACE_FLOW, "JPEGEnc Ddep: program_link total_bitstream_size&0x7Ful: 0x%x\n",total_bitstream_size&0x7Ful);
	OstTraceFiltInst1(TRACE_FLOW, "JPEGEnc Ddep: program_link total_bitstream_size : 0x%x\n",total_bitstream_size);
#endif
	OstTraceFiltInst0(TRACE_API, "JPEGEnc Ddep: program_link exiting \n");

}



/*****************************************************************************/
/**
 * \brief  setNeeds
 *
 * Set to this component memory needs (internal buffers)
  */
/*****************************************************************************/

PUT_PRAGMA
void METH(setNeeds)(t_uint16 channelId, ts_ddep_buffer_descriptor bbm_desc, ts_ddep_buffer_descriptor debugBuffer_desc,t_uint32 temp_buf_address)
{
#ifdef __JPEGDENC_ARM_NMF
	//currently not doing anything for arm-nmf
#else

	/*	Description of buffer needs :
			- bufferDesc0 : linked list mgmt
			- bufferDesc1 : Run Level Buffer
	 		- bufferDesc2 : Internal Info Buffer & Param buffer
	 		- bufferDesc3 : Debug Buffer inside which debug information will be stored
	*/

	/*if (s_state == DDEP_STATE_INVALID) return;*/

    s_channelId = channelId;
	OstTraceInt3(TRACE_API, "JPEGEnc Ddep: Entering setNeeds s_channelId : %d sizes bbm=%X, dbg=%X \n", s_channelId, bbm_desc.nSize, debugBuffer_desc.nSize);
	PRINT_VAR(s_channelId);
    printf("setNeeds sizes bbm=%X, dbg=%X\n", bbm_desc.nSize, debugBuffer_desc.nSize);

	// assuming 16 bit memory allocation from ARM side
	bbm_desc.nSize>>=1;
	debugBuffer_desc.nSize>>=1;

	//{/**/if(bufferDesc0.nSize!=0){/* check for 24 bit accessable memory */t_uint32 SHAREDMEM*ptr=(t_uint32 SHAREDMEM*)bufferDesc0.nMpcAddress;*ptr=0x123456ul;if(*ptr!=0x123456ul){while(avoid_infinte_loop_warning) printf("Not a 24 bit memory\n");}}}

	printf("-1-");
    s_jpege_ddep_desc.bbm_desc = bbm_desc;
	s_jpege_ddep_desc.s_debug_buffer_desc = debugBuffer_desc;

	if (debugBuffer_desc_nMpcAddress != debugBuffer_desc.nMpcAddress)
	{
		mpc_trace_init(debugBuffer_desc.nMpcAddress, debugBuffer_desc.nSize);
		debugBuffer_desc_nMpcAddress = debugBuffer_desc.nMpcAddress;
	}
    iSleep.setDebug(0, s_jpege_ddep_desc.s_debug_buffer_desc.nMpcAddress, 0);


	if (isFWDisabled)
    {//new code change
		codec_algo.setDebug(DBG_MODE_NO_HW, convto16bitmode(s_jpege_ddep_desc.s_debug_buffer_desc.nMpcAddress), convto16bitmode(0));
		vpp_algo.setDebug(DBG_MODE_NO_HW, convto16bitmode(s_jpege_ddep_desc.s_debug_buffer_desc.nMpcAddress), convto16bitmode(0));
	}
	else
	{//existing code
		codec_algo.setDebug(DBG_MODE_NORMAL, convto16bitmode(s_jpege_ddep_desc.s_debug_buffer_desc.nMpcAddress), convto16bitmode(0));
		vpp_algo.setDebug(DBG_MODE_NORMAL, convto16bitmode(s_jpege_ddep_desc.s_debug_buffer_desc.nMpcAddress), convto16bitmode(0));
	}
	OstTraceInt3(TRACE_FLOW, "JPEGEnc Ddep: In setNeeds bbm_desc.nSize : 0x%x bbm_desc.nPhysicalAddress : 0x%x bbm_desc.nLogicalAddress : 0x%x\n",bbm_desc.nSize,bbm_desc.nPhysicalAddress,bbm_desc.nLogicalAddress);
	OstTraceInt3(TRACE_FLOW, "JPEGEnc Ddep: In setNeeds bbm_desc.nMpcAddress  : 0x%x temp_buf_address  : 0x%x s_jpege_ddep_desc.s_debug_buffer_desc.nMpcAddress : 0x%x",bbm_desc.nMpcAddress,temp_buf_address,s_jpege_ddep_desc.s_debug_buffer_desc.nMpcAddress);
	PRINT_VAR(s_jpege_ddep_desc.s_debug_buffer_desc.nMpcAddress);
	PRINT_VAR(bbm_desc.nSize );
	PRINT_VAR(bbm_desc.nPhysicalAddress  );
	PRINT_VAR(bbm_desc.nLogicalAddress   );
	PRINT_VAR(bbm_desc.nMpcAddress       );
    PRINT_VAR(temp_buf_address);
    add_tmp_buff=temp_buf_address;
	PRINT_VAR(add_tmp_buff);
	{
		create_link(bbm_desc);

	}
	set_need_done = 1;
	OstTraceInt0(TRACE_API, "JPEGEnc Ddep: setNeeds Exiting \n");
#endif
} /* End of setNeeds() function. */


PUT_PRAGMA
void METH(setConfig)(t_uint16 channelId)
{
}


STATIC_FLAG t_uint16 COMP(download_parameters)(void)
{

	t_uint16 retry = 0;
    t_uint16 l_atomic_current_conf;

#ifdef __JPEGDENC_ARM_NMF
	#ifdef ENABLE_ARMNMF_LOGS
		printf("\ndownload_parameters enter");
	#endif
#else
		printf("\ndownload_parameters enter");
#endif

	OstTraceInt1(TRACE_API, "JPEGEnc Ddep : In download_parameters, line no :%d \n",__LINE__);

	if (!set_param_done) return 0;

	l_atomic_current_conf = ps_ddep_sec_jpeg_param_desc_dh->host_param_set_atomic_exit;

	if (l_atomic_current_conf == last_read_conf_no) //already using latest configuration
		return 1;

#ifdef __JPEGDENC_ARM_NMF
	#ifdef ENABLE_ARMNMF_LOGS
		printf("\ndownload_parameters::new setting available");
	#endif
#else
		printf("\ndownload_parameters::new setting available");
#endif

	while(1)
	{
		t_uint16 SHAREDMEM * src;
		t_uint16 SHAREDMEM * dest;
		t_uint32 count;
		t_uint16 atomic_entry_exit;
        retry++;
		if (retry > 10) {/*raise error event here*/ return 0;}

		src = (t_uint16 SHAREDMEM *)ps_ddep_sec_jpeg_param_desc_host_set;
		dest = (t_uint16 SHAREDMEM *)ps_ddep_sec_jpeg_param_desc_tmp_set;
		count = sizeof(ts_ddep_sec_jpeg_param_desc_);
		atomic_entry_exit = ps_ddep_sec_jpeg_param_desc_dh->host_param_set_atomic_exit;
		while(count--) *dest++=*src++;
		if (atomic_entry_exit == ps_ddep_sec_jpeg_param_desc_dh->host_param_set_atomic_entry)
		{
			//atomically copied from host to tmp, now copy it to main mpc_param_set
			src = (t_uint16 SHAREDMEM *)ps_ddep_sec_jpeg_param_desc_tmp_set;
			dest= (t_uint16 SHAREDMEM *)ps_ddep_sec_jpeg_param_desc_mpc_set;
			count = sizeof(ts_ddep_sec_jpeg_param_desc_);
			while(count--) *dest++=*src++;
			/*while(count--)
			{
                if (*dest != *src)
				{
					printf("count=%d, %X %X", count, *dest, *src);
					break;
				}
				dest++;
				src++;
			}*/
			last_read_conf_no = atomic_entry_exit;
			OstTraceInt1(TRACE_FLOW, "JPEGEnc Ddep :  In download_parameters new setting captured in %d attempts \n",retry);
#ifdef __JPEGDENC_ARM_NMF
		#ifdef ENABLE_ARMNMF_LOGS
			printf("\nnew setting captured in %d attempts", retry);
		#endif
#else
			printf("\nnew setting captured in %d attempts", retry);
#endif
			break;
		}
			// else retry
	}
	PRINT_VAR(ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.window_width);
	PRINT_VAR(ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.window_height);

#ifdef __JPEGDENC_ARM_NMF
		#ifdef ENABLE_ARMNMF_LOGS
			printf("\ndownload_parameters exit");
		#endif
#else
			printf("\ndownload_parameters exit");
#endif
	OstTraceInt0(TRACE_API, "JPEGEnc Ddep :  In download_parameters exit \n");
	return 1;
}

STATIC_FLAG void COMP(restore_values)(void)
{
	OstTraceFiltInst0(TRACE_API, "JPEGEnc Ddep :  In restore_values enter \n");
	if(frame_height_save)
	ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.frame_height = frame_height_save;
	if(window_height_save)
	ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.window_height = window_height_save;
	if(window_vertical_offset_save)
	ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.window_vertical_offset = window_vertical_offset_save;
	if(header_size)
	ps_ddep_sec_jpeg_param_desc_mpc_set->s_header_buf.header_size = SVP_SHAREDMEM_TO16(header_size);
	/* ER 408297, remove conversion */
	if(addr_header_buffer) {
		ps_ddep_sec_jpeg_param_desc_mpc_set->s_header_buf.addr_header_buffer = /*SVP_SHAREDMEM_TO16*/(t_ahb_address)(addr_header_buffer);
	}
	header_size=0;
	addr_header_buffer=0;
	frame_height_save=0;
	window_height_save=0;
	window_vertical_offset_save=0;
	OstTraceFiltInst0(TRACE_API, "JPEGEnc Ddep :  In restore_values exit \n");
}



PUT_PRAGMA
#ifdef 	__JPEGDENC_ARM_NMF
void METH(setParameter)(t_t1xhv_algo_id algoId, ts_ddep_buffer_descriptor ddep_sec_jpeg_param_desc)
#else
void METH(setParameter)(t_uint16 channelId, t_t1xhv_algo_id algoId, ts_ddep_buffer_descriptor ddep_sec_jpeg_param_desc)
#endif
{
#ifdef __JPEGDENC_ARM_NMF
	OstTraceInt0(TRACE_API, "JPEGEnc Ddep :  In setParameter enter  \n");
	#ifdef ENABLE_ARMNMF_LOGS
		printf("Set Parameters\n");
	#endif
	ps_ddep_sec_jpeg_param_desc_dh = (ts_ddep_sec_jpeg_param_desc_dh *)ddep_sec_jpeg_param_desc.nLogicalAddress;
	ts_ddep_sec_jpeg_param_desc_ *null_desc = (ts_ddep_sec_jpeg_param_desc_ *)NULL;

	/*keep mpc address for dynamic configuration*/
	ps_ddep_sec_jpeg_param_desc_mpc_set  = (ts_ddep_sec_jpeg_param_desc_ *)&ps_ddep_sec_jpeg_param_desc_dh->mpc_param_set;
	ps_ddep_sec_jpeg_param_desc_host_set = (ts_ddep_sec_jpeg_param_desc_ *)&ps_ddep_sec_jpeg_param_desc_dh->host_param_set;
	ps_ddep_sec_jpeg_param_desc_tmp_set  = (ts_ddep_sec_jpeg_param_desc_ *)&ps_ddep_sec_jpeg_param_desc_dh->tmp_param_set;

	param.algoId = algoId;
	/* now set physical addresses*/
	/* OPTIMIZATION POINT */
	param.addr_in_frame_buffer      = (t_uint32)ps_ddep_sec_jpeg_param_desc_dh + ((t_uint32)(&null_desc->s_in_frame_buffer         ));
	param.addr_out_frame_buffer     = (t_uint32)ps_ddep_sec_jpeg_param_desc_dh + ((t_uint32)(&null_desc->s_out_fram_buffer         ));
	param.addr_internal_buffer      = (t_uint32)ps_ddep_sec_jpeg_param_desc_dh + ((t_uint32)(&null_desc->s_internal_buffer         ));
	param.addr_header_buffer     	= (t_uint32)ps_ddep_sec_jpeg_param_desc_dh + ((t_uint32)(&null_desc->s_header_buf     	       ));
	param.addr_in_bitstream_buffer  = (t_uint32)ps_ddep_sec_jpeg_param_desc_dh + ((t_uint32)(&null_desc->s_in_out_bitstream_buffer ));
	param.addr_out_bitstream_buffer = (t_uint32)ps_ddep_sec_jpeg_param_desc_dh + ((t_uint32)(&null_desc->s_in_out_bitstream_buffer ));
	param.addr_in_parameters        = (t_uint32)ps_ddep_sec_jpeg_param_desc_dh + ((t_uint32)(&null_desc->s_in_parameters           ));
	param.addr_out_parameters       = (t_uint32)ps_ddep_sec_jpeg_param_desc_dh + ((t_uint32)(&null_desc->s_out_parameters          ));
	param.addr_in_frame_parameters  = (t_uint32)ps_ddep_sec_jpeg_param_desc_dh + ((t_uint32)(&null_desc->s_in_out_frame_parameters ));
	param.addr_out_frame_parameters = (t_uint32)ps_ddep_sec_jpeg_param_desc_dh + ((t_uint32)(&null_desc->s_in_out_frame_parameters ));
#else
	t_uint32 physical_address_desc = ddep_sec_jpeg_param_desc.nPhysicalAddress;
	ts_ddep_sec_jpeg_param_desc_ SHAREDMEM *null_desc = (ts_ddep_sec_jpeg_param_desc_ SHAREDMEM *)NULL;
	OstTraceInt0(TRACE_FLOW, "JPEGEnc Ddep :  In setParameter enter  \n");
	printf("\nsetParameter() enter");

	/*keep mpc address for dynamic configuration*/
	ps_ddep_sec_jpeg_param_desc_dh = (ts_ddep_sec_jpeg_param_desc_dh SHAREDMEM *)ddep_sec_jpeg_param_desc.nMpcAddress;
	ps_ddep_sec_jpeg_param_desc_mpc_set = (ts_ddep_sec_jpeg_param_desc_ SHAREDMEM *)&ps_ddep_sec_jpeg_param_desc_dh->mpc_param_set;
	ps_ddep_sec_jpeg_param_desc_host_set = (ts_ddep_sec_jpeg_param_desc_ SHAREDMEM *)&ps_ddep_sec_jpeg_param_desc_dh->host_param_set;
	ps_ddep_sec_jpeg_param_desc_tmp_set = (ts_ddep_sec_jpeg_param_desc_ SHAREDMEM *)&ps_ddep_sec_jpeg_param_desc_dh->tmp_param_set;

	PRINT_VAR(ps_ddep_sec_jpeg_param_desc_dh);
	PRINT_VAR(ps_ddep_sec_jpeg_param_desc_mpc_set);
	PRINT_VAR(ps_ddep_sec_jpeg_param_desc_host_set);
	PRINT_VAR(ps_ddep_sec_jpeg_param_desc_tmp_set);

	PRINT_VAR(SVP_SHAREDMEM_FROM16(ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_frame_buffer.addr_source_buffer));
	PRINT_VAR(SVP_SHAREDMEM_FROM16(ps_ddep_sec_jpeg_param_desc_mpc_set->s_ddep_in_param.nSliceHeight));
	PRINT_VAR(SVP_SHAREDMEM_FROM16(ps_ddep_sec_jpeg_param_desc_mpc_set->s_ddep_in_param.nStride));
	param.algoId = algoId;

	/* now set physical addresses*/
	/*OPTIMIZATION POINT*/
	param.addr_in_frame_buffer      = physical_address_desc + ((t_uint32)(&null_desc->s_in_frame_buffer         ))*(t_uint32)2;
	param.addr_out_frame_buffer     = physical_address_desc + ((t_uint32)(&null_desc->s_out_fram_buffer         ))*(t_uint32)2;
	param.addr_internal_buffer      = physical_address_desc + ((t_uint32)(&null_desc->s_internal_buffer         ))*(t_uint32)2;
	param.addr_header_buffer     	= physical_address_desc + ((t_uint32)(&null_desc->s_header_buf     	        ))*(t_uint32)2;
	param.addr_in_bitstream_buffer  = physical_address_desc + ((t_uint32)(&null_desc->s_in_out_bitstream_buffer ))*(t_uint32)2;
	param.addr_out_bitstream_buffer = physical_address_desc + ((t_uint32)(&null_desc->s_in_out_bitstream_buffer ))*(t_uint32)2;
	param.addr_in_parameters        = physical_address_desc + ((t_uint32)(&null_desc->s_in_parameters           ))*(t_uint32)2;
	param.addr_out_parameters       = physical_address_desc + ((t_uint32)(&null_desc->s_out_parameters          ))*(t_uint32)2;
	param.addr_in_frame_parameters  = physical_address_desc + ((t_uint32)(&null_desc->s_in_out_frame_parameters ))*(t_uint32)2;
	param.addr_out_frame_parameters = physical_address_desc + ((t_uint32)(&null_desc->s_in_out_frame_parameters ))*(t_uint32)2;
#endif

	set_param_done = 1;
	get_codec_parameter=1;
	last_read_conf_no = 0;
	OstTraceInt3(TRACE_FLOW, "JPEGEnc Ddep : In setParameter, set_param_done : %d,get_codec_parameter : %d,last_read_conf_no : %d\n",set_param_done,get_codec_parameter,last_read_conf_no);
	download_parameters();

#ifdef 	__JPEGDENC_ARM_NMF
//+ER 354962
	iCommandAck.pendingCommandAck();
//-ER 354962

	#ifdef ENABLE_ARMNMF_LOGS
		printf("\nsetParameter() exit");
	#endif
#else
		printf("\nsetParameter() exit");
#endif

	OstTraceInt1(TRACE_API, "JPEGEnc Ddep : In setParameter Done, line no :%d \n",__LINE__);
}


PUT_PRAGMA
void METH(set_nNumber)(t_uint32 nNumber_t)
{
#ifdef 	__JPEGDENC_ARM_NMF
	// need to check
	isHVAbased = nNumber_t & 0x01 ;
    nNumber_t  = nNumber_t >> 1;
	nNumber_a=nNumber_t;
#endif
}


#ifdef 	__JPEGDENC_ARM_NMF
	//nothing to do
#else
	#pragma force_dcumode
#endif
void METH(eventEndAlgo)(t_t1xhv_status status,t_t1xhv_encoder_info errors,t_uint32 durationInTicks,t_uint32 bitstream_size)
{
    t_uint32 flags = 0,actualsizeReq=0,addedbyte=0;

    #ifndef 	__JPEGDENC_ARM_NMF
	t_uint16 mod = 0;
    #endif
	OstTraceFiltInst0(TRACE_API, "JPEGEnc Ddep :  eventEndAlgo Entering \n");
	OstTraceFiltInst3(TRACE_DEBUG, "JPEGEnc Ddep : In eventEndAlgo status : 0x%x and bitstreamSize : 0x%x total_bitstream_size : 0x%x",status,bitstream_size,total_bitstream_size);
	OstTraceFiltInst2(TRACE_DEBUG, "JPEGEnc Ddep : Value of Error : %d HANDLE_SEND_COMMAND : %d \n",errors,HANDLE_SEND_COMMAND);

#ifdef 	__JPEGDENC_ARM_NMF
	#ifdef ENABLE_ARMNMF_LOGS
	    printf("\n eventEndAlgo entering");
	#endif
#else
	    printf("\n eventEndAlgo entering");
#endif

	PRINT_VAR(errors);
	PRINT_VAR(status);
	PRINT_VAR(bitstream_size);
    if(status==STATUS_BUFFER_NEEDED)
	{
#ifdef 	__JPEGDENC_ARM_NMF
	//nothgin done currently
#else
		total_bitstream_size -=16*SVP_SHAREDMEM_FROM24(bufOut_slice->allocLen);
		bufOut_slice->filledLen=SVP_SHAREDMEM_FROM24(bufOut_slice->allocLen);
#endif
		OstTraceFiltInst0(TRACE_FLOW, "JPEGEnc Ddep :  eventEndAlgo STATUS_BUFFER_NEEDED \n");
		PRINT_VAR(total_bitstream_size);
	    ReleaseBuffer(1,bufOut_slice);
		bufOut_slice=0;
#ifdef 	__JPEGDENC_ARM_NMF
	if(buffer_available_atoutput())
#else
        if(buffer_available_atoutput(&mDataDep))
#endif
		{
			OstTraceFiltInst0(TRACE_DEBUG, "JPEGEnc Ddep :  eventEndAlgo STATUS_BUFFER_NEEDED BUffer Available\n");
#ifdef __JPEGDENC_ARM_NMF
		    bufOut_slice->nFilledLen = SVP_SHAREDMEM_TO24(0);
#else
	bufOut_slice->filledLen = SVP_SHAREDMEM_TO24(0);
#endif
			update_link();
        }
		else
		{
            SET_FLAG_FOR_EOW=1;
			OstTraceFiltInst0(TRACE_DEBUG, "JPEGEnc Ddep :  eventEndAlgo buffer not present \n");
#ifdef 	__JPEGDENC_ARM_NMF
		#ifdef ENABLE_ARMNMF_LOGS
			printf("\n  buffer not present");
		#endif
#else
			printf("\n  buffer not present");
#endif
		}
	}
	else if(SET_FLAG_FOR_EOW)
	{
		OstTraceFiltInst0(TRACE_FLOW, "JPEGEnc Ddep :  eventEndAlgo else if(SET_FLAG_FOR_EOW) \n");
		SET_FLAG_FOR_EOW=0;
		if (bufIn) ReleaseBuffer(0,bufIn);
		bufIn=0;
		 release_resources();
	}
	else
	{
#ifdef __JPEGDENC_ARM_NMF
		if (isEncodingUnderProgress)
			isEncodingUnderProgress = OMX_FALSE;
#endif
	OstTraceFiltInst2(TRACE_FLOW, "JPEGEnc Ddep :  eventEndAlgo bufIn : 0x%x bufOut_slice : 0x%x\n",(t_uint32)bufIn,(t_uint32)bufOut_slice);
	PRINT_VAR(bufIn);
	PRINT_VAR(bufOut_slice);
#ifdef __JPEGDENC_ARM_NMF
	if (bufIn) flags = bufIn->nFlags;
#else
	if (bufIn) flags = bufIn->flags;
#endif

	if (bufIn) ReleaseBuffer(0,bufIn);
	bufIn=0;

	if (bufOut_slice)
	{
		// Get the filled len + byteinlastword
		bitstream_size = convfrom16bitmode(bitstream_size);
		total_bitstream_size += bitstream_size;
		OstTraceFiltInst1(TRACE_DEBUG, "JPEGEnc Ddep :  eventEndAlgo total_bitstream_size : 0x%x\n",total_bitstream_size);
        PRINT_VAR(total_bitstream_size);

#ifndef __JPEGDENC_ARM_NMF
		if(isFWDisabled)
		{
			total_bitstream_size=0;
			bitstream_size=0;
		}
#endif
        //if (ddep_jpege_slice_type == DDEP_JPEGE_SLICE_TYPE_LAST_AND_FIRST || ddep_jpege_slice_type == DDEP_JPEGE_SLICE_TYPE_LAST)
		if (ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.last_slice)
		{
#ifdef 	__JPEGDENC_ARM_NMF
		if(bitstream_size>8*(bufOut_slice->nAllocLen))
		{
			actualsizeReq=(bitstream_size+7)/8;
			if(actualsizeReq%256!=0)
			{
				addedbyte=256-(actualsizeReq%256);
				actualsizeReq = actualsizeReq + addedbyte;
			}
					OstTraceFiltInst0(TRACE_FLOW, "JPEGEnc Ddep :  eventEndAlgo calling portsettings changed \n");
			iChangePortSettingsSignal.portSettings(actualsizeReq);
			bufOut_slice->nFilledLen = 0;
		}
		else
			bufOut_slice->nFilledLen = (total_bitstream_size+7)/8; //New
#else
		if(bitstream_size>2*(8*(bufOut_slice->allocLen)))
		{
			actualsizeReq=(bitstream_size+15)/16;
			if(actualsizeReq%256!=0)
			{
				addedbyte=256-(actualsizeReq%256);
				actualsizeReq = actualsizeReq + addedbyte;
			}
			OstTraceFiltInst0(TRACE_FLOW, "JPEGEnc Ddep :  eventEndAlgo calling portsettings changed \n");
			iChangePortSettingsSignal.portSettings(actualsizeReq*2);
			bufOut_slice->filledLen = SVP_SHAREDMEM_TO24(0);
		}
		else
			bufOut_slice->filledLen = SVP_SHAREDMEM_TO24((total_bitstream_size+15)/ 16);


			mod = (total_bitstream_size % 16);
			if (mod >= 1 && mod <= 8) {
				 bufOut_slice->byteInLastWord = 1;
			} else {
				 bufOut_slice->byteInLastWord = 2;
			}
#endif
#if 0
#ifdef 	__JPEGDENC_ARM_NMF
			bufOut_slice->nFilledLen = (total_bitstream_size+7)/8; //New
#else
			t_uint16 mod;
			bufOut_slice->filledLen = SVP_SHAREDMEM_TO24((total_bitstream_size+15)/ 16);
			mod = (total_bitstream_size % 16);
			if (mod >= 1 && mod <= 8) {
				 bufOut_slice->byteInLastWord = 1;
			} else {
				 bufOut_slice->byteInLastWord = 2;
			}
#endif
#endif
#ifdef 	__JPEGDENC_ARM_NMF
			bufOut_slice->nFlags = flags;
			if (bufOut_slice->nFlags&BUFFERFLAG_EOS)
#else
			bufOut_slice->flags = flags;
			if (bufOut_slice->flags&BUFFERFLAG_EOS)
#endif
			{

#ifdef 	__JPEGDENC_ARM_NMF
				bufOut_slice->nFlags |= BUFFERFLAG_ENDOFRAME;
#else
				bufOut_slice->flags |= BUFFERFLAG_ENDOFRAME;
#endif
				ReleaseBuffer(1,bufOut_slice);
#ifdef 	__JPEGDENC_ARM_NMF
				OstTraceFiltInst1(TRACE_FLOW, "JPEGEnc Ddep :  eventEndAlgo calling eventHandler Output Bufer Flag Value : %d\n",bufOut_slice->nFlags);
				proxy.eventHandler(OMX_EventBufferFlag, 1, bufOut_slice->nFlags);
#else
				OstTraceFiltInst1(TRACE_FLOW, "JPEGEnc Ddep :  eventEndAlgo calling eventHandler Output Bufer Flag Value : %d\n",bufOut_slice->flags);
				proxy.eventHandler(OMX_EventBufferFlag, 1, bufOut_slice->flags);
#endif
				bufOut_slice=0;
			}
			else if(current_top==frame_height)
			{
#ifdef 	__JPEGDENC_ARM_NMF
				bufOut_slice->nFlags |= BUFFERFLAG_ENDOFRAME;
#else
				bufOut_slice->flags |= BUFFERFLAG_ENDOFRAME;
#endif
				ReleaseBuffer(1,bufOut_slice);
				bufOut_slice=0;
			}
            total_bitstream_size = 0;
		}
		else if(HANDLE_SEND_COMMAND)  //this is not for EOW
		{
			HANDLE_SEND_COMMAND=0;
						if(bufOut_slice)
						{
							ReleaseBuffer(1,bufOut_slice);

						#ifdef 	__JPEGDENC_ARM_NMF
							bufOut_slice->nFilledLen = SVP_SHAREDMEM_TO24(0);
						#else
							bufOut_slice->filledLen = SVP_SHAREDMEM_TO24(0);
						#endif
							bufOut_slice=0;
						}
						if(bufIn)
						{
							 ReleaseBuffer(0,bufIn);
							 bufIn=0;
						#ifdef 	__JPEGDENC_ARM_NMF
							 bufIn->nFilledLen = SVP_SHAREDMEM_TO24(0);
						#else
							 bufIn->filledLen = SVP_SHAREDMEM_TO24(0);
						#endif
						}
		}

	}
	// Should we really released the HW on a buffer based
	//if(current_top!=frame_height)
    release_resources();

	if(defer_pause)
	{
		defer_pause = 0;
#ifdef 	__JPEGDENC_ARM_NMF
	Component::sendCommand(OMX_CommandStateSet, OMX_StatePause);
#else
	Component_sendCommand(&mDataDep, OMX_CommandStateSet, OMX_StatePause);
#endif
	}


	iSendLogEvent.eventProcess();

	}
	//handling send command in case of EOW

	if(HANDLE_SEND_COMMAND && SET_FLAG_FOR_EOW)//when to set::dont know?
	{
			linkin_loop();
            SET_FLAG_FOR_EOW=0;
			HANDLE_SEND_COMMAND=0;
   }

	restore_values();//do we really need this or this is taken care in processactual
	OstTraceFiltInst0(TRACE_API, "JPEGEnc Ddep :  eventEndAlgo exiting \n");
#ifdef 	__JPEGDENC_ARM_NMF
	#ifdef ENABLE_ARMNMF_LOGS
		printf("\n eventEndAlgo exiting");
	#endif
#else
		printf("\n eventEndAlgo exiting");
#endif
    return;
}




void COMP(disablePortIndication)(t_uint32 portIdx)
{
	OstTraceFiltInst1(TRACE_API, "JPEGEnc Ddep: disablePortIndication entering for port : %d\n",portIdx);
#ifdef 	__JPEGDENC_ARM_NMF
	#ifdef ENABLE_ARMNMF_LOGS
    	printf("\n disablePortIndication");
    #endif
#else
    	printf("\n disablePortIndication");
#endif

	release_resources();
	OstTraceFiltInst1(TRACE_API, "JPEGEnc Ddep: disablePortIndication exiting for port : %d\n",portIdx);
}

 void COMP(enablePortIndication)(t_uint32 portIdx)
{
	#ifdef ENABLE_ARMNMF_LOGS
		printf("\n enablePortIndication");
	#endif

	release_resources();
	OstTraceFiltInst1(TRACE_API, "JPEGEnc Ddep: enablePortIndication exiting for port : %d\n",portIdx);
}

void COMP(flushPortIndication)(t_uint32 portIdx)
{
#ifdef ENABLE_ARMNMF_LOGS
	printf("\n flushPortIndication");
#endif
	release_resources();
	OstTraceFiltInst1(TRACE_API, "JPEGEnc Ddep: flushPortIndication exiting for port : %d\n",portIdx);
}

void COMP(stateChangeIndication)(OMX_STATETYPE oldState, OMX_STATETYPE newState)
{
#ifdef ENABLE_ARMNMF_LOGS
	printf("\n stateChangeIndication");
#endif
	OstTraceFiltInst0(TRACE_API, "JPEGEnc Ddep: stateChangeIndication \n");
}


// Process function is called only if component is in executing mode
#ifdef __JPEGDENC_ARM_NMF
void jpegenc_arm_nmf_parser::process()
#else
static void process(Component *this)
#endif
{
	OstTraceFiltInst0(TRACE_API, "JPEGEnc Ddep: process Entering\n");
#ifdef __JPEGDENC_ARM_NMF
	#ifdef ENABLE_ARMNMF_LOGS
    	printf("\n process entering");
	#endif
#else
    	printf("\n process entering");
#endif
    if (get_hw_resource)
	{
		// resource already aquired and in use
		OstTraceFiltInst0(TRACE_DEBUG, "JPEGEnc Ddep: process exiting (get_hw_resource) \n");
#ifdef __JPEGDENC_ARM_NMF
	#ifdef ENABLE_ARMNMF_LOGS
		printf("\n process exiting (get_hw_resource)");
	#endif
#else
		printf("\n process exiting (get_hw_resource)");
#endif

		return;
	}

	if (get_hw_resource_requested)
	{
		// resource already requested and in use
		OstTraceFiltInst0(TRACE_DEBUG, "JPEGEnc Ddep: process exiting (get_hw_resource_requested) \n");
#ifdef __JPEGDENC_ARM_NMF
	#ifdef ENABLE_ARMNMF_LOGS
		printf("\n process exiting (get_hw_resource_requested)");
	#endif
#else
		printf("\n process exiting (get_hw_resource_requested)");
#endif
		return;
	}

	// Check buffers and Parameter availabilities//some problem with this code??
#ifdef __JPEGDENC_ARM_NMF
	if ( get_codec_parameter && buffer_available_atinput() && buffer_available_atoutput())

#else
    if ( get_codec_parameter && buffer_available_atinput(&mDataDep) && buffer_available_atoutput(&mDataDep))
#endif
	{		// Call to RM to get HW the ressource
		OstTraceFiltInst0(TRACE_DEBUG, "JPEGEnc Ddep: iResource.allocResource entering \n");
#ifdef __JPEGDENC_ARM_NMF
		#ifdef ENABLE_ARMNMF_LOGS
			printf("\n iResource.allocResource entering");
		#endif
#else
			printf("\n iResource.allocResource entering");
#endif

		if (ddep_jpege_slice_type == DDEP_JPEGE_SLICE_TYPE_LAST || ddep_jpege_slice_type == DDEP_JPEGE_SLICE_TYPE_LAST_AND_FIRST)
			download_parameters();
		OstTraceFiltInst0(TRACE_DEBUG, "JPEGEnc Ddep: Abt to call Process Actual from process \n");
#ifdef __JPEGDENC_ARM_NMF
				#ifdef ENABLE_ARMNMF_LOGS
                        printf("Abt to call Process Actual \n");
				#endif
			processActual();
#else
		get_hw_resource_requested=1;
		if(ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.sampling_mode==2)
		{

			iResource.allocResource(RESOURCE_VPP, &iInformResourceStatus);
		}
		else
		{
			iResource.allocResource(RESOURCE_MMDSP, &iInformResourceStatus);
		}
		OstTraceFiltInst0(TRACE_DEBUG, "JPEGEnc Ddep: iResource.allocResource exiting \n");
		printf("\n iResource.allocResource exiting");
#endif
	}
#ifdef __JPEGDENC_ARM_NMF
	#ifdef ENABLE_ARMNMF_LOGS
		printf("\n process exiting");
	#endif
#else
		printf("\n process exiting");
#endif
		OstTraceFiltInst0(TRACE_API, "JPEGEnc Ddep: process Exiting\n");
}



#ifdef __JPEGDENC_ARM_NMF
void jpegenc_arm_nmf_parser::processActual()
#else
static void processActual(Component *this)
#endif
{

	t_sint16 current_slice_height=0;
    t_uint8 skip_encoding = 0;
	t_uint32 flags;
	restore_values();

	if (!bufIn)
		return;

	OstTraceFiltInst1(TRACE_API, "JPEGEnc Ddep :  In ProcessActual entering, line no :%d \n",__LINE__);

#ifdef __JPEGDENC_ARM_NMF
	#ifdef ENABLE_ARMNMF_LOGS
		printf("\n processActual entering");
	#endif
#else
		printf("\n processActual entering");
#endif

	{
		// All  resource already available , go !

		// Output Buffer ready , so reserve it ,
		if (ddep_jpege_slice_type == DDEP_JPEGE_SLICE_TYPE_LAST || ddep_jpege_slice_type == DDEP_JPEGE_SLICE_TYPE_LAST_AND_FIRST)
		{
			pending_height = ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.frame_height;
			ddep_jpege_slice_type = DDEP_JPEGE_SLICE_TYPE_FIRST;
			current_top = 0;

			total_bitstream_size = 0;
			if (bufOut_slice)
			{
#ifdef __JPEGDENC_ARM_NMF
			bufOut_slice->nFlags = bufIn->nFlags;
			bufOut_slice->nTimeStamp = bufIn->nTimeStamp;
#else
			bufOut_slice->nTimeStamph = bufIn->nTimeStamph;
			bufOut_slice->nTimeStampl = bufIn->nTimeStampl;
#endif
		}
		}
		else if (ddep_jpege_slice_type == DDEP_JPEGE_SLICE_TYPE_FIRST_SKIPPED)
			ddep_jpege_slice_type = DDEP_JPEGE_SLICE_TYPE_FIRST;
		else ddep_jpege_slice_type = DDEP_JPEGE_SLICE_TYPE_SUBSEQUENT;

		{
			t_uint32 min_slice_unit_size, nb_of_slice_units = 0;

			min_slice_unit_size = SVP_SHAREDMEM_FROM16(ps_ddep_sec_jpeg_param_desc_mpc_set->s_ddep_in_param.nSliceHeight) * SVP_SHAREDMEM_FROM16(ps_ddep_sec_jpeg_param_desc_mpc_set->s_ddep_in_param.nStride);


#ifdef __JPEGDENC_ARM_NMF
				if (bufIn) nb_of_slice_units = bufIn->nFilledLen / min_slice_unit_size;
#else
			nb_of_slice_units = (SVP_SHAREDMEM_FROM24(bufIn->filledLen)<<1) / min_slice_unit_size;
		   OstTraceFiltInst3(TRACE_DEBUG, "JPEGEnc Ddep :  In ProcessActual min_slice_unit_size : %d  nb_of_slice_units : %d SVP_SHAREDMEM_FROM24(bufIn->filledLen) : 0x%x\n",min_slice_unit_size,nb_of_slice_units,SVP_SHAREDMEM_FROM24(bufIn->filledLen));
           PRINT_VAR(SVP_SHAREDMEM_FROM24(bufIn->filledLen));
	       PRINT_VAR(min_slice_unit_size);
		   PRINT_VAR(nb_of_slice_units);
#endif



			if (pending_height < SVP_SHAREDMEM_FROM16(ps_ddep_sec_jpeg_param_desc_mpc_set->s_ddep_in_param.nSliceHeight))
				current_slice_height = SVP_SHAREDMEM_FROM16(ps_ddep_sec_jpeg_param_desc_mpc_set->s_ddep_in_param.nSliceHeight);
			else current_slice_height = nb_of_slice_units * SVP_SHAREDMEM_FROM16(ps_ddep_sec_jpeg_param_desc_mpc_set->s_ddep_in_param.nSliceHeight);
		}

		//save to restore back
		frame_height_save = ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.frame_height;
		window_height_save = ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.window_height;
		window_vertical_offset_save = ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.window_vertical_offset;
		frame_height = ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.frame_height;
       // PRINT_VAR(current_slice_height);
        if (current_slice_height==0)
		{
			skip_encoding = 1;
		}
		else
		{
			pending_height -= current_slice_height;

			ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.frame_height = current_slice_height;

			if (current_top >= window_vertical_offset_save)
				ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.window_vertical_offset = 0;
			else ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.window_vertical_offset = window_vertical_offset_save - current_top;

			if (current_slice_height <= ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.window_vertical_offset)
			{
				skip_encoding = 1;
			}
			else
			{
				if (current_top + current_slice_height < window_vertical_offset_save + window_height_save)
					ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.window_height = current_slice_height - ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.window_vertical_offset;
				else
				{
					if ((window_vertical_offset_save + window_height_save) > (current_top + ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.window_vertical_offset))
					{
						ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.window_height = (window_vertical_offset_save + window_height_save) - (current_top + ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.window_vertical_offset);
					}
					else skip_encoding = 1;
				}
			}

			current_top += current_slice_height;

			if (pending_height <= 0)
			{
				if (ddep_jpege_slice_type == DDEP_JPEGE_SLICE_TYPE_FIRST)
					ddep_jpege_slice_type = DDEP_JPEGE_SLICE_TYPE_LAST_AND_FIRST;
				else ddep_jpege_slice_type = DDEP_JPEGE_SLICE_TYPE_LAST;
				OstTraceFiltInst0(TRACE_DEBUG, "JPEGEnc Ddep :  In ProcessActual, DDEP_JPEGE_SLICE_TYPE_LAST(_AND_FIRST)\n");
#ifdef __JPEGDENC_ARM_NMF
		#ifdef ENABLE_ARMNMF_LOGS
				printf("DDEP_JPEGE_SLICE_TYPE_LAST(_AND_FIRST)");
		#endif
#else
				printf("DDEP_JPEGE_SLICE_TYPE_LAST(_AND_FIRST)");
#endif
			}
		}
		OstTraceFiltInst1(TRACE_FLOW, "JPEGEnc Ddep :  In ProcessActual, skip_encoding : %d \n",skip_encoding);
		PRINT_VAR(skip_encoding);
		PRINT_VAR("\n skip_encoding value\n ");

		if (skip_encoding == 0)
		{
			ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.last_slice = 0;

			if (ddep_jpege_slice_type == DDEP_JPEGE_SLICE_TYPE_LAST || ddep_jpege_slice_type == DDEP_JPEGE_SLICE_TYPE_LAST_AND_FIRST)
				ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.last_slice = 1;

			if (current_top>=window_vertical_offset_save+window_height_save)
				ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.last_slice = 1;


			program_link(s_jpege_ddep_desc.bbm_desc);

			header_size =  SVP_SHAREDMEM_FROM16(ps_ddep_sec_jpeg_param_desc_mpc_set->s_header_buf.header_size);
			/* ER 408297, remove conversion */
			addr_header_buffer =  /*SVP_SHAREDMEM_FROM16*/(t_ahb_address)(ps_ddep_sec_jpeg_param_desc_mpc_set->s_header_buf.addr_header_buffer);

			if (ddep_jpege_slice_type == DDEP_JPEGE_SLICE_TYPE_FIRST || ddep_jpege_slice_type == DDEP_JPEGE_SLICE_TYPE_LAST_AND_FIRST)
			{
				// reset in-out parameters per frame
				ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_out_frame_parameters.restart_mcu_count = ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.restart_interval;
				ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_out_frame_parameters.dc_predictor_y = 0;
				ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_out_frame_parameters.dc_predictor_cb = 0;
				ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_out_frame_parameters.dc_predictor_cr = 0;
				ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_out_frame_parameters.restart_marker_id= 0;
			}
			else
			{
				ps_ddep_sec_jpeg_param_desc_mpc_set->s_header_buf.header_size = SVP_SHAREDMEM_TO16(0);
				ps_ddep_sec_jpeg_param_desc_mpc_set->s_header_buf.addr_header_buffer = SVP_SHAREDMEM_TO16(0);
			}

		#ifdef __JPEGDENC_ARM_NMF
			 if(bufIn->nFlags&BUFFERFLAG_EOS)
		#else
			 if(bufIn->flags&BUFFERFLAG_EOS)
 		#endif
			{
                if (ddep_jpege_slice_type == DDEP_JPEGE_SLICE_TYPE_FIRST)
					ddep_jpege_slice_type = DDEP_JPEGE_SLICE_TYPE_LAST_AND_FIRST;
				else ddep_jpege_slice_type = DDEP_JPEGE_SLICE_TYPE_LAST;
				ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.last_slice = 1;
			}

		    if(add_tmp_buff)
			 {
				OstTraceFiltInst1(TRACE_DEBUG, "JPEGEnc Ddep :  In ProcessActual, add_tmp_buff (in case of YUV420Planar) : 0x%x \n",add_tmp_buff);
				 PRINT_VAR(add_tmp_buff);
				 ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_frame_buffer.addr_source_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(add_tmp_buff));
			 }
			 else
			 {
			 #ifdef __JPEGDENC_ARM_NMF
				 ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_frame_buffer.addr_source_buffer = (t_ahb_address)SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(bufIn->pBuffer));
			 #else
				 ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_frame_buffer.addr_source_buffer = SVP_SHAREDMEM_TO16(SVP_SHAREDMEM_FROM24(bufIn->address));
			 #endif

			 }

			 OstTraceFiltInst0(TRACE_DEBUG, "JPEGEnc_Ddep :  In processActual, Input Parameter Values\n");
			 OstTraceFiltInst1(TRACE_DEBUG, "JPEGEnc_Ddep :  In processActual, Frame Width : %d\n",ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.frame_width);
			 OstTraceFiltInst1(TRACE_DEBUG, "JPEGEnc_Ddep :  In processActual, Frame Height : %d\n",ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.frame_height);
			 OstTraceFiltInst1(TRACE_DEBUG, "JPEGEnc_Ddep :  In processActual, Window Width : %d\n",ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.window_width);
			 OstTraceFiltInst1(TRACE_DEBUG, "JPEGEnc_Ddep :  In processActual, Window Height : %d\n",ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.window_height);
			 OstTraceFiltInst1(TRACE_DEBUG, "JPEGEnc_Ddep :  In processActual, Window Horizontal Offset : %d\n",ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.window_horizontal_offset);
			 OstTraceFiltInst1(TRACE_DEBUG, "JPEGEnc_Ddep :  In processActual, Window Vertical Offset : %d\n",ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.window_vertical_offset);
			 OstTraceFiltInst1(TRACE_DEBUG, "JPEGEnc_Ddep :  In processActual, Rotation : %d\n",ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.rotation);
			 OstTraceFiltInst1(TRACE_DEBUG, "JPEGEnc_Ddep :  In processActual, Sampling Mode : %d\n",ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.sampling_mode);
			 OstTraceFiltInst1(TRACE_DEBUG, "JPEGEnc_Ddep :  In processActual, Restart Interval : %d\n",ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.restart_interval);
			 OstTraceFiltInst1(TRACE_DEBUG, "JPEGEnc_Ddep :  In processActual, Is Last Slice : %d\n",ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.last_slice);

#ifdef __JPEGDENC_ARM_NMF
		#ifdef ENABLE_ARMNMF_LOGS
			printf("\n codec_algo.configureAlgo");
		#endif
#else
			printf("\n codec_algo.configureAlgo");
#endif


			// asynchronous call
#ifdef __JPEGDENC_ARM_NMF

	isEncodingUnderProgress = OMX_TRUE;

#ifdef __JPEGDENC_NDK_5500_
				if(isHVAbased)
				{
					memcpy((OMX_U8 *)((tps_t1xhv_bitstream_buf_pos)param.addr_in_bitstream_buffer)->addr_bitstream_start,(OMX_U8 *)((tps_t1xhv_vec_header_buf)param.addr_header_buffer)->addr_header_buffer,((tps_t1xhv_vec_header_buf)param.addr_header_buffer)->header_size);
					OMX_U32 bufPhysicalAddrSrcBuffer,bufPhysicalAddrBtsBuffer;
					((MMHwBuffer *)bufIn->pInputPortPrivate)->PhysAddress((OMX_U32)bufIn->pBuffer, bufIn->nAllocLen,bufPhysicalAddrSrcBuffer);

		            ((tps_t1xhv_vec_frame_buf_in)param.addr_in_frame_buffer)->addr_source_buffer = bufPhysicalAddrSrcBuffer;

					((MMHwBuffer *)bufOut_slice->pOutputPortPrivate)->PhysAddress((OMX_U32)bufOut_slice->pBuffer, bufOut_slice->nAllocLen,bufPhysicalAddrBtsBuffer);
//					memset((void *)(((tps_t1xhv_bitstream_buf_pos)param.addr_in_bitstream_buffer)->addr_bitstream_start + ((tps_t1xhv_vec_header_buf)param.addr_header_buffer)->header_size),0xAB,3500);
		            //iAlgo.sendPhysicalAddress(bufPhysicalAddrSrcBuffer,bufPhysicalAddrBtsBuffer);

		            ((tps_t1xhv_bitstream_buf_pos)param.addr_in_bitstream_buffer)->addr_bitstream_start = bufPhysicalAddrBtsBuffer;
					OstTraceFiltInst1(TRACE_FLOW, "JPEGEnc Ddep :  In ProcessActual, Source Buffer Address : 0x%x \n",bufPhysicalAddrSrcBuffer);
					OstTraceFiltInst1(TRACE_FLOW, "JPEGEnc Ddep :  In ProcessActual, Destination Buffer Address : 0x%x \n",bufPhysicalAddrBtsBuffer);
					OstTraceFiltInst2(TRACE_FLOW, "JPEGEnc Ddep :  In ProcessActual, Header Size : 0x%x, Header Buffer Address : 0x%x \n",((tps_t1xhv_vec_header_buf)param.addr_header_buffer)->header_size,((tps_t1xhv_vec_header_buf)param.addr_header_buffer)->addr_header_buffer);
			    }
#else

			 OstTraceFiltInst1(TRACE_DEBUG, "JPEGEnc Ddep :  In ProcessActual, Source Buffer Address : 0x%x \n",ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_frame_buffer.addr_source_buffer);
			 OstTraceFiltInst1(TRACE_DEBUG, "JPEGEnc Ddep :  In ProcessActual, Destination Buffer Address : 0x%x \n",ps_ddep_sec_jpeg_param_desc_mpc_set->s_out_fram_buffer.addr_dest_buffer);
			 OstTraceFiltInst2(TRACE_DEBUG, "JPEGEnc Ddep :  In ProcessActual, Header Size : 0x%x, Header Buffer Address : 0x%x \n",ps_ddep_sec_jpeg_param_desc_mpc_set->s_header_buf.header_size, ps_ddep_sec_jpeg_param_desc_mpc_set->s_header_buf.addr_header_buffer);
#endif
			 OstTraceFiltInst0(TRACE_DEBUG, "JPEGEnc Ddep :  In ProcessActual, Calling codec_algo.configureAlgo \n");
			iAlgo.configureAlgo(param.addr_in_frame_buffer,
								param.addr_out_frame_buffer,
								param.addr_internal_buffer,
								param.addr_header_buffer,
								param.addr_in_bitstream_buffer,
								param.addr_out_bitstream_buffer,
								param.addr_in_parameters,
								param.addr_out_parameters,
								param.addr_in_frame_parameters,
								param.addr_out_frame_parameters);

			// Start  also synchronous
			#ifdef ENABLE_ARMNMF_LOGS
				printf("\n codec_algo.controlAlgo entering");
			#endif
			OstTraceFiltInst0(TRACE_FLOW, "JPEGEnc Ddep : In ProcessActual codec_algo.controlAlgo entering \n");

			iAlgo.controlAlgo(CMD_START, 1);
#else
			 OstTraceFiltInst1(TRACE_DEBUG, "JPEGEnc Ddep :  In ProcessActual, Source Buffer Address : 0x%x \n",ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_frame_buffer.addr_source_buffer);
			 OstTraceFiltInst1(TRACE_DEBUG, "JPEGEnc Ddep :  In ProcessActual, Destination Buffer Address : 0x%x \n",ps_ddep_sec_jpeg_param_desc_mpc_set->s_out_fram_buffer.addr_dest_buffer);
			 OstTraceFiltInst2(TRACE_DEBUG, "JPEGEnc Ddep :  In ProcessActual, Header Size : 0x%x, Header Buffer Address : 0x%x \n",ps_ddep_sec_jpeg_param_desc_mpc_set->s_header_buf.header_size, ps_ddep_sec_jpeg_param_desc_mpc_set->s_header_buf.addr_header_buffer);
			 OstTraceFiltInst0(TRACE_DEBUG, "JPEGEnc Ddep :  In ProcessActual, Calling codec_algo.configureAlgo \n");

			codec_algo.configureAlgo(&iEndAlgo,
			convto16bitmode(param.addr_in_frame_buffer|EXT_BIT),
			convto16bitmode(param.addr_out_frame_buffer|EXT_BIT),
			convto16bitmode(param.addr_internal_buffer|EXT_BIT),
			convto16bitmode(param.addr_header_buffer|EXT_BIT),
			convto16bitmode(param.addr_in_bitstream_buffer|EXT_BIT),
			convto16bitmode(param.addr_out_bitstream_buffer|EXT_BIT),
			convto16bitmode(param.addr_in_parameters|EXT_BIT),
			convto16bitmode(param.addr_out_parameters|EXT_BIT),
			convto16bitmode(param.addr_in_frame_parameters|EXT_BIT),
			convto16bitmode(param.addr_out_frame_parameters|EXT_BIT));

			// Start  also asynchronous
			printf("\n codec_algo.controlAlgo entering");
			OstTraceFiltInst0(TRACE_DEBUG, "JPEGEnc Ddep : In ProcessActual codec_algo.controlAlgo entering \n");

			STORE_CONTROL_ALGO_TICKS(ps_ddep_sec_jpeg_param_desc_mpc_set/*, _from_ticks*/);

			codec_algo.controlAlgo(CMD_START, s_channelId, 1);
#endif
#ifdef __JPEGDENC_ARM_NMF
		#ifdef ENABLE_ARMNMF_LOGS
			printf("\n codec_algo.controlAlgo exiting");
		#endif
#else
			printf("\n codec_algo.controlAlgo exiting");
#endif

		}
		else
		{
			OstTraceFiltInst0(TRACE_FLOW, "JPEGEnc Ddep : In ProcessActual, encoding Skipped \n");
#ifdef __JPEGDENC_ARM_NMF
	        flags=bufIn->nFlags;
#else
	        flags=bufIn->flags;
#endif
			if (bufIn)
			{
					ReleaseBuffer(0,bufIn);
					bufIn=0;
			}
            if(bufOut_slice)
			{
				if(flags&BUFFERFLAG_EOS||frame_height==current_top)
				{
				#ifdef __JPEGDENC_ARM_NMF
					bufOut_slice->nFlags=flags;
					if(bufOut_slice->nFilledLen)
					bufOut_slice->nFlags=flags|BUFFERFLAG_ENDOFRAME;
					if(flags&BUFFERFLAG_EOS)
					proxy.eventHandler(OMX_EventBufferFlag, 1, bufOut_slice->nFlags);
				#else
					bufOut_slice->flags=flags;
					if(bufOut_slice->filledLen)
					bufOut_slice->flags=flags|BUFFERFLAG_ENDOFRAME;
					if(flags&BUFFERFLAG_EOS)
					proxy.eventHandler(OMX_EventBufferFlag, 1, bufOut_slice->flags);
				#endif

                    			if (ddep_jpege_slice_type == DDEP_JPEGE_SLICE_TYPE_FIRST)
						ddep_jpege_slice_type = DDEP_JPEGE_SLICE_TYPE_LAST_AND_FIRST;
					else
						ddep_jpege_slice_type = DDEP_JPEGE_SLICE_TYPE_LAST;
                    			ReleaseBuffer(1,bufOut_slice);
					bufOut_slice=0;
				}

			}
			if (ddep_jpege_slice_type == DDEP_JPEGE_SLICE_TYPE_FIRST)
				ddep_jpege_slice_type = DDEP_JPEGE_SLICE_TYPE_FIRST_SKIPPED;

			release_resources();
			OstTraceFiltInst0(TRACE_DEBUG, "JPEGEnc Ddep : In ProcessActual, calling process() again \n");
#ifdef __JPEGDENC_ARM_NMF
		#ifdef ENABLE_ARMNMF_LOGS
			printf("CAlling process fnt again from here \n");
        #endif
			process();
#else
            process(this); //try to see if all dependencies are ok. if so re-request resources (optimize-why release if reaquiring)
#endif

		}

	}
#ifdef __JPEGDENC_ARM_NMF
	#ifdef ENABLE_ARMNMF_LOGS
		printf("\n processActual exiting");
	#endif
#else
		printf("\n processActual exiting");
#endif
	OstTraceFiltInst0(TRACE_API, "JPEGEnc Ddep : ProcessActual Exiting \n");
}



PUT_PRAGMA
void METH(setTunnelStatus)(t_sint16 portIdx, t_uint16 isTunneled)
{
	int i=0;
	OstTraceInt1(TRACE_API, "JPEGEnc Ddep : setTunnelStatus Entering Port Id: %d \n",portIdx);
#ifdef __JPEGDENC_ARM_NMF
    if (portIdx == -1)
    {
        for (int i=0; i<NB_PORTS; i++)
        {
            if (isTunneled & (1<<i))
            {
                mPorts[i].setTunnelStatus(true);
            }
            else
            {
                mPorts[i].setTunnelStatus(false);
            }
        }
    }
    else
    {
        if (isTunneled & (1<<portIdx))
        {
            mPorts[portIdx].setTunnelStatus(true);
        }
        else
        {
            mPorts[portIdx].setTunnelStatus(false);
        }
    }

#else

    //Port_setTunnelStatus(&mPorts[portIdx],isTunneled);
    if (portIdx == -1)
    {
        for (i=0; i<NB_PORTS; i++)
        {
            if (isTunneled & (1<<i))
            {
                //mPort[i].setTunnelStatus(true);
                Port_setTunnelStatus(&mPorts[i],true);
            }
            else
            {
                //mPort[i].setTunnelStatus(false);
                Port_setTunnelStatus(&mPorts[i],false);
            }
        }
    }
    else
    {
        if (isTunneled & (1<<portIdx))
        {
			Port_setTunnelStatus(&mPorts[portIdx],true);
            //mPort[portIdx].setTunnelStatus(true);
        }
        else
        {
			Port_setTunnelStatus(&mPorts[portIdx],false);
            //mPort[portIdx].setTunnelStatus(false);
        }
    }

	printf("\n setTunnelStatus");
#endif
	OstTraceInt1(TRACE_API, "JPEGEnc Ddep : setTunnelStatus Exiting Port Id: %d \n",portIdx);
}

PUT_PRAGMA
#ifdef NEWFSMINIT
void METH(fsmInit)(fsmInit_t initFSM)
#else
void METH(fsmInit)(t_uint16 portsDisabled,t_uint16 portsTunneled)
#endif
{
#ifdef NEWFSMINIT
    t_uint16 portsDisabled = initFSM.portsDisabled;
    t_uint16 portsTunneled = initFSM.portsTunneled;
#endif

#ifdef 	__JPEGDENC_ARM_NMF

    if (initFSM.traceInfoAddr)
    {
        setTraceInfo(initFSM.traceInfoAddr, initFSM.id1);
	}

	mPorts[0].init(InputPort, /* port direction */
								 false, /* isBufferSupplier */
								 true, /* isHWPort */
								 0, /* port we're sharing with */
								 0, /* initialBuffers ptrFIXME: mBufIn_list*/
								 8, /* bufferCount */
								 &inputport, /* return interface */
								 0, /* portIdx */
								 portsDisabled&1, /* isDisabled */
								 portsTunneled&1,
								 (Component *)this /* componentOwner */
								 );

	mPorts[1].init(OutputPort, /* port direction */
								 false, /* isBufferSupplier */
								 true, /* isHWPort */
								 0, /* port we're sharing with */
								 0, /* initialBuffers ptr FIXME: mBufOut_list*/
								 8, /* bufferCount */
								 &outputport, /* return interface */
								 1, /* portIdx */
								 (portsDisabled >> 1)&1, /* isDisabled */
								 (portsTunneled>>1)&1,
								 (Component *)this /* componentOwner */
								 );
    init(NB_PORTS, mPorts, &proxy, &me, 0); //FIXME:invalid parameter is to be initilaized properly

#else
    if (initFSM.traceInfoAddr)
    {
//		this->mTraceInfoPtr->dspAddr = initFSM.traceInfoAddr;
	//	this->mTraceInfoPtr->traceEnable = initFSM.traceInfoAddr->traceEnable;
		//this->mId1 = initFSM.id1;
		FSM_traceInit(&mDataDep, initFSM.traceInfoAddr, initFSM.id1);
		this = (TRACE_t *)&mDataDep;
//		this->mTraceInfoPtr =  initFSM.traceInfoAddr;
 //       this->mId1 = initFSM.id1;
	}
	OstTraceInt3(TRACE_FLOW,"JPEGEnc Ddep : In fsminit depAdd (0x%x)  traceEnable (0x%x)   id1 (0x%x) ",this->mTraceInfoPtr->dspAddr,this->mTraceInfoPtr->traceEnable,this->mId1);

	// Port are non supplier , fifo size is one ,
	Port_init(&mPorts[0], InputPort, false, 1, NULL, &mFifoIn, 8, &inputport, 0, portsDisabled&1,portsTunneled&1, &mDataDep);
	Port_init(&mPorts[1], OutputPort,false, 1, NULL, &mFifoOut, 8, &outputport, 1, (portsDisabled>>1)&1,(portsTunneled>>1)&1, &mDataDep);

	mDataDep.reset      = reset;
	mDataDep.process    = process;
	mDataDep.disablePortIndication = disablePortIndication;
	mDataDep.enablePortIndication = enablePortIndication;
	mDataDep.flushPortIndication = flushPortIndication;

	Component_init(&mDataDep, NB_PORTS, mPorts, &proxy);
#endif
}


PUT_PRAGMA
void METH(sendCommand)(OMX_COMMANDTYPE cmd, t_uword param)
{
    OstTraceFiltInst1(TRACE_API,"JPEGEnc Ddep : sendCommand received <at line %d>",__LINE__);
#ifdef 	__JPEGDENC_ARM_NMF
	if(cmd == OMX_CommandStateSet)
	{
		 switch((OMX_STATETYPE)param)
			{
		         case OMX_StatePause:
					        if(isEncodingUnderProgress)
					        {
					        	 defer_pause = 1;
								 return;
							 }
					break;

				default:
					break;
			}
	}
	Component::sendCommand(cmd, param);

#else
    OstTraceFiltInst2(TRACE_FLOW,"JPEGEnc Ddep : sendCommand sendCommand entering cmd=%d, param=%d",(t_uint32)cmd, (t_uint32)param);
	printf("\nsendCommand entering cmd=%d, param=%d", (t_uint32)cmd, (t_uint32)param);
    if (cmd == OMX_CommandFlush && (param !=1)) //omx_all handled
	{
         ddep_jpege_slice_type = DDEP_JPEGE_SLICE_TYPE_LAST;
	}

	if(cmd==OMX_CommandStateSet)
	{
		 switch((OMX_STATETYPE)param)
			{
         case OMX_StateIdle:
         				if (compState == OMX_StateExecuting)
		 				{
							OstTraceFiltInst0(TRACE_DEBUG,"JPEGEnc Ddep : sendCommand iSleep.allowSleep(&iInformResourceStatus)\n");
		 					iSleep.allowSleep(&iInformResourceStatus);
		 					printf("\n iSleep.allowSleep(&iInformResourceStatus)");
		 				}

         				compState=(OMX_STATETYPE)param;
         				break;

         case OMX_StatePause:
						if (compState == OMX_StateExecuting)
						{
							OstTraceFiltInst0(TRACE_DEBUG,"JPEGEnc Ddep : sendCommand iSleep.allowSleep(&iInformResourceStatus)\n");
							iSleep.allowSleep(&iInformResourceStatus);
							printf("\n iSleep.allowSleep(&iInformResourceStatus)");
						}
						compState=(OMX_STATETYPE)param;
					        if((get_hw_resource || get_hw_resource_requested || get_vpp_resource))
					         {
							 defer_pause = 1;
							 OstTraceFiltInst1(TRACE_DEBUG,"JPEGEnc Ddep : sendCommand defer pause : %d \n",defer_pause);
							 PRINT_VAR(defer_pause);
							 return;
						 //printf("\n delay sendCommand for pause");
						}

						break;

		 case OMX_StateExecuting:
						if (compState != OMX_StateExecuting)
						{
							OstTraceFiltInst0(TRACE_DEBUG,"JPEGEnc Ddep : sendCommand iSleep.preventSleep(&iInformResourceStatus \n");
							iSleep.preventSleep(&iInformResourceStatus);
							printf("\n iSleep.preventSleep(&iInformResourceStatus)");
						}

			 			compState=(OMX_STATETYPE)param;
             			break;

					default:
						ASSERT(0);
			}
    }

	//handling send command in case of EOW   ::Same code to present in endalgo also
	if((cmd == OMX_CommandPortDisable)||(cmd == OMX_CommandFlush)||(cmd==OMX_CommandStateSet && compState == OMX_StateIdle))
	{

	    HANDLE_SEND_COMMAND=1;

		if(SET_FLAG_FOR_EOW && get_hw_resource)
		{
			linkin_loop();
            HANDLE_SEND_COMMAND=0;
		}
		if((bufOut_slice || bufIn )&& (!get_hw_resource) &&(!get_hw_resource_requested))
		{
			HANDLE_SEND_COMMAND=0;
			if(bufOut_slice && (param !=0)) //idle also being taken care
			{
				ReleaseBuffer(1,bufOut_slice);
				bufOut_slice=0;
				bufOut_slice->filledLen = SVP_SHAREDMEM_TO24(0);
			}
			if(bufIn && (param !=1)) //idle also being taken care
			{
				ReleaseBuffer(0,bufIn);
				bufIn=0;
				bufIn->filledLen = SVP_SHAREDMEM_TO24(0);
			}
		 }
		ddep_jpege_slice_type = DDEP_JPEGE_SLICE_TYPE_LAST;

		if((!bufOut_slice) && (!bufIn))
		{
			HANDLE_SEND_COMMAND=0;
		}
	}
	printf("\nsendCommand exiting1");
	Component_sendCommand(&mDataDep, cmd, param);
	printf("\nsendCommand exiting");
#endif
    OstTraceFiltInst1(TRACE_API,"JPEGEnc Ddep : sendCommand exiting <at line %d>",__LINE__);
}



#ifdef 	__JPEGDENC_ARM_NMF
	//nothing to do
#else
	#pragma force_dcumode
#endif
void METH(eventProcess)()
{
    OstTraceFiltInst1(TRACE_API,"JPEGEnc Ddep : eventProcess entering <at line %d>",__LINE__);
#ifdef __JPEGDENC_ARM_NMF
	#ifdef ENABLE_ARMNMF_LOGS
		printf("\n eventProcess entering");
	#endif
#else
		printf("\n eventProcess entering");
#endif
	if (get_hw_resource)
	{
		OstTraceFiltInst0(TRACE_FLOW,"JPEGEnc Ddep : eventProcess exiting (get_hw_resource)");
		// resource already aquired and in use
#ifdef __JPEGDENC_ARM_NMF
	#ifdef ENABLE_ARMNMF_LOGS
		printf("\n process exiting (get_hw_resource)");
	#endif
#else
		printf("\n process exiting (get_hw_resource)");
#endif
		return;
	}

	if (get_hw_resource_requested)
	{
		OstTraceFiltInst0(TRACE_FLOW,"JPEGEnc Ddep : eventProcess exiting (get_hw_resource_requested)");
		// resource already requested and in use
#ifdef __JPEGDENC_ARM_NMF
	#ifdef ENABLE_ARMNMF_LOGS
		printf("\n process exiting (get_hw_resource_requested)");
	#endif
#else
		printf("\n process exiting (get_hw_resource_requested)");
#endif
		return;
	}
		//buffer_available_atinput(&mDataDep)
	// Check buffers and Parameter availabilities
#ifdef __JPEGDENC_ARM_NMF
	if ( get_codec_parameter && buffer_available_atinput() && buffer_available_atoutput())

#else
    if ( get_codec_parameter && buffer_available_atinput(&mDataDep) && buffer_available_atoutput(&mDataDep))
#endif
	{
		OstTraceFiltInst0(TRACE_DEBUG, "JPEGEnc Ddep: eventProcess iResource.allocResource entering \n");

		// Call to RM to get HW the ressource
#ifdef __JPEGDENC_ARM_NMF
	#ifdef ENABLE_ARMNMF_LOGS
		printf("\n iResource.allocResource entering");
	#endif
#else
		printf("\n iResource.allocResource entering");
#endif

		if (ddep_jpege_slice_type == DDEP_JPEGE_SLICE_TYPE_LAST || ddep_jpege_slice_type == DDEP_JPEGE_SLICE_TYPE_LAST_AND_FIRST)
			download_parameters();


		OstTraceFiltInst0(TRACE_DEBUG, "JPEGEnc Ddep: eventProcess Abt to call process Actual from here \n");
#ifdef __JPEGDENC_ARM_NMF
	#ifdef ENABLE_ARMNMF_LOGS
		printf ("Abt to call process Actual from here \n");
	#endif
			processActual();
#else

		get_hw_resource_requested=1;

		iResource.allocResource(RESOURCE_MMDSP, &iInformResourceStatus);
		printf("\n iResource.allocResource exiting");
#endif
		OstTraceFiltInst0(TRACE_DEBUG, "JPEGEnc Ddep: eventProcess iResource.allocResource exiting \n");
	}
#ifdef __JPEGDENC_ARM_NMF
	#ifdef ENABLE_ARMNMF_LOGS
		printf("\n eventProcess exiting");
	#endif
#else
		printf("\n eventProcess exiting");
#endif
		OstTraceFiltInst1(TRACE_DEBUG,"JPEGEnc Ddep : eventProcess exiting <at line %d>",__LINE__);
}


#ifdef __JPEGDENC_ARM_NMF
void jpegenc_arm_nmf_parser::processEvent(void)
{
        Component::processEvent();
}

void jpegenc_arm_nmf_parser::emptyThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer)
{
		OstTraceFiltInst1(TRACE_FLOW,"JPEGEnc Ddep : emptyThisBuffer  empty this buffer = %x\n",(t_uint32)pBuffer);
		#ifdef ENABLE_ARMNMF_LOGS
			printf("\nARM NMF empty this buffer = %x\n",pBuffer);
		#endif
		Component::deliverBuffer(0, pBuffer);
}

void jpegenc_arm_nmf_parser::fillThisBuffer(OMX_BUFFERHEADERTYPE_p pBuffer)
{
	OstTraceFiltInst1(TRACE_FLOW,"JPEGEnc Ddep : fillThisBuffer  empty this buffer = %x\n",(t_uint32)pBuffer);
	#ifdef ENABLE_ARMNMF_LOGS
		printf("\nARM NMF fill this buffer = %x\n",pBuffer);
	#endif
       Component::deliverBuffer(1, pBuffer);
}


jpegenc_arm_nmf_parser::jpegenc_arm_nmf_parser()
{
	OstTraceInt0(TRACE_API,"JPEGEnc Ddep : jpegenc_arm_nmf_parser contructor\n");
	#ifdef ENABLE_ARMNMF_LOGS
		printf("jpegenc_arm_nmf_parser::jpegenc_arm_nmf_parser\n");
	#endif
	set_param_done = 0;
	last_read_conf_no = 0;
    total_bitstream_size = 0;
    current_top = 0;
    frame_height = 0;
	window_vertical_offset_save = 0;
	window_height_save = 0;
	frame_height_save = 0;
	pending_height = 0;
	current_slice_height = 0;
	ddep_jpege_slice_type = DDEP_JPEGE_SLICE_TYPE_LAST;
	bufIn = NULL;
	bufOut_slice = NULL;
	SET_FLAG_FOR_EOW = 0;
	nNumber_a=0;
	isHVAbased = 0; // false need to check
	HANDLE_SEND_COMMAND = 0;
	get_hw_resource = 0;
	get_vpp_resource = 0;
	get_hw_resource_requested = 0;
	defer_pause = 0;
	header_size = 0;
	isEncodingUnderProgress = OMX_FALSE;
	addr_header_buffer = 0;
	add_tmp_buff = 0;
	compState = OMX_StateIdle;

    param.ready = 0;
    param.algoId = ID_SEC_JPEG;
    param.addr_in_frame_buffer = 0;
    param.addr_out_frame_buffer = 0;
    param.addr_internal_buffer = 0;
    param.addr_header_buffer = 0;
    param.addr_in_bitstream_buffer = 0;
    param.addr_out_bitstream_buffer = 0;
    param.addr_in_parameters = 0;
    param.addr_out_parameters = 0;
    param.addr_in_frame_parameters = 0;
    param.addr_out_frame_parameters = 0;
	ps_arm_nmf_jpeg_bitstream_buf_link.addr_next_buf_link = 0;
	ps_arm_nmf_jpeg_bitstream_buf_link.addr_prev_buf_link = 0;
	ps_arm_nmf_jpeg_bitstream_buf_link.addr_buffer_start = 0;
	ps_arm_nmf_jpeg_bitstream_buf_link.addr_buffer_end = 0;
    //s_jpege_ddep_desc = 0x0;
    get_codec_parameter = 0;
    isDataEncoded = OMX_FALSE;

    s_jpege_ddep_desc.bbm_desc.nSize = 0;
    s_jpege_ddep_desc.bbm_desc.nPhysicalAddress = 0;
    s_jpege_ddep_desc.bbm_desc.nLogicalAddress = 0;
    s_jpege_ddep_desc.bbm_desc.nMpcAddress = 0;

	s_jpege_ddep_desc.s_debug_buffer_desc.nSize = 0;
	s_jpege_ddep_desc.s_debug_buffer_desc.nPhysicalAddress = 0;
	s_jpege_ddep_desc.s_debug_buffer_desc.nLogicalAddress = 0;
	s_jpege_ddep_desc.s_debug_buffer_desc.nMpcAddress = 0;
}



jpegenc_arm_nmf_parser::~jpegenc_arm_nmf_parser()
{

}

t_nmf_error jpegenc_arm_nmf_parser::construct(void)
{
	OstTraceInt0(TRACE_API,"JPEGEnc Ddep : jpegenc_arm_nmf_parser::contruct\n");
	#ifdef ENABLE_ARMNMF_LOGS
		printf("construct called\n");
	#endif
	return NMF_OK;
}

void jpegenc_arm_nmf_parser::destroy()
{

}

OMX_BOOL jpegenc_arm_nmf_parser::buffer_available_atinput()
{
	if((bufIn == NULL) && (mPorts[0].queuedBufferCount()))
	{
		#ifdef ENABLE_ARMNMF_LOGS
			printf("\n  remove buffer - input");
		#endif
		bufIn = mPorts[0].dequeueBuffer();
		OstTraceFiltInst0(TRACE_FLOW, "JPEGEnc Ddep  New Buffer available at Input \n");
		return OMX_TRUE;
	}
	else if(bufIn)
		return OMX_TRUE;
	else
		return OMX_FALSE;
}


OMX_BOOL jpegenc_arm_nmf_parser::buffer_available_atoutput()
{
	if((bufOut_slice==NULL) && (mPorts[1].queuedBufferCount()))
	{
		#ifdef ENABLE_ARMNMF_LOGS
			printf("\n  remove buffer - output");
		#endif
		bufOut_slice = mPorts[1].dequeueBuffer();
		OstTraceFiltInst0(TRACE_FLOW, "JPEGEnc Ddep  Buffer available at Output \n");
		return OMX_TRUE;
	}
	else if(bufOut_slice)
		return OMX_TRUE;
	else
		return OMX_FALSE;

}
#else



void METH(eventReleasebuffer)(t_uint32 port_idx,Buffer_p buf)
{
     OmxEvent ev;
	 OstTraceInt1(TRACE_API, "JPEGEnc Ddep  eventReleasebuffer entering port index: %d\n",port_idx);
	printf("\nentering eventReleaseBuffer bufferport_index=%d",(t_uint32)port_idx);
    ev.fsmEvent.signal =  OMX_RETURNBUFFER_SIG;
    ev.args.returnBuffer.portIdx = port_idx;
    ev.args.returnBuffer.buffer = buf;
	FSM_dispatch(&mDataDep,&ev);

	OstTraceInt0(TRACE_API, "JPEGEnc Ddep  eventReleasebuffer exiting \n");
}


#define disableFWCodeexection METH(disableFWCodeexection)
#pragma force_dcumode
void disableFWCodeexection(t_bool value) {
	isFWDisabled = value;
	OstTraceInt0(TRACE_API, "JPEGEnc Ddep  disableFWCodeexection exiting \n");
}

#pragma force_dcumode
void METH(vppendAlgo)(t_t1xhv_status status,t_t1xhv_vpp_info info,t_uint32 durationInTicks)
{
	OstTraceFiltInst0(TRACE_API, "JPEGEnc Ddep  vppendAlgo entering \n");
    printf("enter endAlgo from vpp\n");
	PRINT_VAR(status);
	PRINT_VAR(info);
    ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.sampling_mode=4;
	release_resources();
	iResource.allocResource(RESOURCE_MMDSP, &iInformResourceStatus);
    printf("exit endAlgofrom vpp\n");
	OstTraceFiltInst0(TRACE_API, "JPEGEnc Ddep  vppendAlgo exiting \n");
}

#pragma force_dcumode
void METH(processEvent)() {
	OstTraceFiltInst0(TRACE_API, "JPEGEnc Ddep  processEvent entering \n");
	printf("\nprocessEvent entering");
    Component_processEvent(&mDataDep);
	printf("\nprocessEvent exiting");
	OstTraceFiltInst0(TRACE_API, "JPEGEnc Ddep  processEvent exiting \n");

}

#pragma force_dcumode
void METH(emptyThisBuffer)(Buffer_p buf) {
	OstTraceInt0(TRACE_API, "JPEGEnc Ddep  emptyThisBuffer entering \n");
	printf("\nemptyThisBuffer entering");
    Component_deliverBuffer(&mDataDep, 0, buf);
	printf("\nemptyThisBuffer exiting");
	OstTraceInt0(TRACE_API, "JPEGEnc Ddep  emptyThisBuffer exiting \n");
}
#pragma force_dcumode
void METH(fillThisBuffer)(Buffer_p buf) {
	OstTraceInt0(TRACE_API, "JPEGEnc Ddep  fillThisBuffer entering \n");
	printf("\nfillThisBuffer entering");
    Component_deliverBuffer(&mDataDep, 1, buf);
    if(SET_FLAG_FOR_EOW)
	{
    SET_FLAG_FOR_EOW=0;
	buffer_available_atoutput(&mDataDep);
    update_link();
	}
	printf("\nfillThisBuffer exiting");
	OstTraceInt0(TRACE_API, "JPEGEnc Ddep  fillThisBuffer exiting \n");
}




/*****************************************************************************/
/**
 * \brief  Get Ressource status from RM
 *
 *
  */
/*****************************************************************************/

#pragma force_dcumode
void METH(informResourceStatus)(t_t1xhv_resource_status status,t_t1xhv_resource resource)
{
	OstTraceInt0(TRACE_API, "JPEGEnc Ddep  iSendLogEvent informResourceStatus entering \n");
	printf("\niSendLogEvent informResourceStatus");
	OstTraceInt0(TRACE_FLOW, "JPEGEnc Ddep : informResourceStatus calling event_informResourceStatus \n");
	iSendLogEvent.event_informResourceStatus(status,resource);
}


#pragma force_dcumode
void METH(event_informResourceStatus)(t_t1xhv_resource_status status,t_t1xhv_resource resource)
{
	FsmEvent ev;
	OstTraceFiltInst2(TRACE_API, "JPEGEnc Ddep  event_informResourceStatus entering status : %d resource : %d\n",status,resource);
	OstTraceFiltInst1(TRACE_DEBUG, "JPEGEnc Ddep  event_informResourceStatus sampling mode : %d\n",ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.sampling_mode);
	printf("\n informResourceStatus");
	PRINT_VAR(status);
	PRINT_VAR(resource);
    if(ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.sampling_mode==1)//Sampling mode is 1 for OMX_COLOR_FormatYUV420Planar
	{
		OstTraceFiltInst0(TRACE_FLOW, "JPEGEnc Ddep  event_informResourceStatus entering  if YUV420lanar\n");
		switch (status)
		{
			case STA_RESOURCE_FREE    :
				get_vpp_resource=1;
        	    vpp_algo.configureAlgoFormatConv(&ivppEndAlgo,
										YUV420RasterPlaner_YUV420MB,
                                        SVP_SHAREDMEM_TO16(bufIn->address),// 0,
										SVP_SHAREDMEM_TO16(add_tmp_buff),//0,
										0,
										0,
										ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.frame_width,
										ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.frame_height,
										ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.window_width,
										ps_ddep_sec_jpeg_param_desc_mpc_set->s_in_parameters.window_height);
				OstTraceFiltInst0(TRACE_DEBUG, "JPEGEnc Ddep : vpp_algo.controlAlgo entering \n");
				printf("\n vpp_algo.controlAlgo entering");
				vpp_algo.controlAlgo(CMD_START, s_channelId, 1);
				printf("\n vpp_algo.controlAlgo exiting");
				OstTraceFiltInst0(TRACE_DEBUG, "JPEGEnc Ddep : vpp_algo.controlAlgo exiting \n");
                break;
			default:
				break;
		}
	}
	else
	{
		OstTraceFiltInst0(TRACE_DEBUG, "JPEGEnc Ddep  event_informResourceStatus entering  if not YUV420lanar\n");
    	switch (status)
    	{
        	case STA_RESOURCE_FREE    :
        	    // A previously allocated resource is now free, so ready to be used.
        	    // This will call the function process if componentis still in Executing mode
        	    // Cannot launch directly the decoder because may be the component is no more in
        	    // executing
        	    get_hw_resource=1;
				get_hw_resource_requested=0;
				if (compState == OMX_StatePause)
				{
					OstTraceFiltInst3(TRACE_DEBUG, "JPEGEnc Ddep : informResourceStatus port1=%d port2=%d state =%d\n",Port_queuedBufferCount(&mPorts[0]),Port_queuedBufferCount(&mPorts[1]),compState);
					 printf("\n informResourceStatus port1=%d port2=%d state =%d ",Port_queuedBufferCount(&mPorts[0]),Port_queuedBufferCount(&mPorts[1]),compState);
					release_resources();
				}
				else
					processActual(&mDataDep);
        	    // PASCAL           ev.signal = FSM_GET_RESOURCE_SIG;
        	    // PASCAL            FSM_dispatch(&mDataDep, &ev);

        	    break;

        	case STA_PARAM_ERROR      : // Input parameter error so that allocation can't be done.
        	    break;
        	//----case STA_NOT_SUPPORTED    : // Required allocation is not supported.
        	//----    break;
        	//----case STA_ERROR_ALLOC_FULL : // Internal allocation FIFO full. Please allocate later.
        	//----    break;

    		default:
    			break;
  		}
	}
}


#pragma force_dcumode
void METH(start)()
{
}

#endif



#ifdef NONE
/* dummy define for editor */
#define SHAREDMEM
#endif

