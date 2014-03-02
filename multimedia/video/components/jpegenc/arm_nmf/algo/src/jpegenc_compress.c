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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <malloc.h>
#include <math.h>	/*used only by BlockDCT*/

#include "types.h"
#ifdef __NMF
#include <t1xhv_retarget.idt>
#endif

#define EOB_EVENT 0
#define EOB1_EVENT 0x10

#define MONOCHROME 0
#define FORMAT420 1
#define FORMAT422 2
#define FORMAT444 3
#define FORMAT422INT 5
//4 is specifically kept for OMX_COLOR_FormatYUV420MBPackedSemiPlanar
#include "jpegenc_compress.h"
#include "jpegenc_fdct_20_16.h"
#include "jpegenc_init.h"

#ifdef __NMF
#include <t1xhv_vec_jpeg.idt>
#else
#include "host_interface_jpegenc.h"
#endif

#ifdef TRACE_ENABLE
t_uint16 jpegenc_check_error(tps_t1xhv_vec_jpeg_session Gps_vec_jpeg_session);
#endif
/*****************************************************
*
*  void jpeg_init_info(void)
*
*  initialize all the data and scales the quantization tables
*
*****************************************************/
void jpegenc_init_info(tps_t1xhv_vec_jpeg_session Gps_vec_jpeg_session)
{
	/*initialize JPEG data*/
	t_sint32 i,j;
	t_uint32 offset_byte_from_start, nStartBits;
	t_uint8 first_byte;
	t_uint16 *YQTable_hamac , *CQTable_hamac;
	tps_t1xhv_bitstream_buf_pos    Gps_jpeg_bitstream_buf_pos	= Gps_vec_jpeg_session->Gps_jpeg_bitstream_buf_pos;
	tps_t1xhv_vec_jpeg_param_in    Gps_vec_jpeg_param_in		= Gps_vec_jpeg_session->Gps_vec_jpeg_param_in;
	tps_t1xhv_vec_jpeg_global      Gps_t1xhv_vec_jpeg_global	= &Gps_vec_jpeg_session->Gs_t1xhv_vec_jpeg_global;
	tps_t1xhv_bitstream_buf_link   Gps_jpeg_bitstream_buf_link	= (tps_t1xhv_bitstream_buf_link)Gps_jpeg_bitstream_buf_pos->addr_bitstream_buf_struct;
	//	buf_link_info = (tps_t1xhv_bitstream_buf_link *)Gps_jpeg_bitstream_buf_pos->addr_bitstream_buf_struct;

	offset_byte_from_start = Gps_jpeg_bitstream_buf_pos->bitstream_offset/8;
	nStartBits = (Gps_jpeg_bitstream_buf_pos->bitstream_offset & 0x7);


	Gps_t1xhv_vec_jpeg_global->bitstreamBufferSize = Gps_jpeg_bitstream_buf_link->addr_buffer_end - Gps_jpeg_bitstream_buf_link->addr_buffer_start;

	YQTable_hamac = Gps_t1xhv_vec_jpeg_global->YQTable_hamac;
	CQTable_hamac = Gps_t1xhv_vec_jpeg_global->CQTable_hamac;
#ifdef TRACE_ENABLE
	//printf("\n width = %d height = %d\n",Gps_vec_jpeg_param_in->frame_width,Gps_vec_jpeg_param_in->frame_height);
#endif

	if (Gps_vec_jpeg_param_in->sampling_mode==MONOCHROME)
	{
		Gps_t1xhv_vec_jpeg_global->MCU_WIDTH=8;
		Gps_t1xhv_vec_jpeg_global->MCU_HEIGHT=8;
	}
	else
	{
		Gps_t1xhv_vec_jpeg_global->MCU_WIDTH = ((Gps_vec_jpeg_param_in->sampling_mode==FORMAT444) ? 8 : 16);
		Gps_t1xhv_vec_jpeg_global->MCU_HEIGHT = ((Gps_vec_jpeg_param_in->sampling_mode==FORMAT420) ? 16 : 8);
	}

	/*create the buffer for output*/
	Gps_t1xhv_vec_jpeg_global->output_bytes =  (t_uint8 *)(Gps_jpeg_bitstream_buf_pos->addr_bitstream_start + offset_byte_from_start);

#ifdef TRACE_ENABLE
	//printf("Log: offset_byte_from_start = %d, nStartBits=%d, first_byte=%x jpg_put_buffer=%d\n",offset_byte_from_start,nStartBits,Gps_t1xhv_vec_jpeg_global->output_bytes[0], Gps_t1xhv_vec_jpeg_global->jpg_put_buffer);
#endif

	if(nStartBits)
	{
		first_byte = Gps_t1xhv_vec_jpeg_global->output_bytes[0];
		first_byte = first_byte >> (8-nStartBits);
		Gps_t1xhv_vec_jpeg_global->jpg_put_buffer = (first_byte << (24 - nStartBits)) ;

#ifdef TRACE_ENABLE
		//printf("first_byte=%x\n",first_byte);
		//printf("jpg_put_buffer=%x\n",Gps_t1xhv_vec_jpeg_global->jpg_put_buffer);
#endif
		Gps_t1xhv_vec_jpeg_global->jpg_put_bits = nStartBits;
	}
	else
	{
		Gps_t1xhv_vec_jpeg_global->jpg_put_buffer = 0;
		Gps_t1xhv_vec_jpeg_global->jpg_put_bits = 0;
	}

	Gps_t1xhv_vec_jpeg_global->next_output_byte=0;
	Gps_t1xhv_vec_jpeg_global->nTotalBits = 0;
	Gps_t1xhv_vec_jpeg_global->nStartBits = nStartBits;
	//memcpy(&(YQTable_hamac),&(Gps_vec_jpeg_param_in->quant_luma),64*sizeof(t_uint16));
	//memcpy(&(CQTable_hamac),&(Gps_vec_jpeg_param_in->quant_chroma),64*sizeof(t_uint16));

	for(i=0,j=0;i<64;i=i+8,j++)
		{
			YQTable_hamac[0+j] = Gps_vec_jpeg_param_in->quant_luma[i];
			YQTable_hamac[8+j] = Gps_vec_jpeg_param_in->quant_luma[i+1];
			YQTable_hamac[16+j] = Gps_vec_jpeg_param_in->quant_luma[i+2];
			YQTable_hamac[24+j] = Gps_vec_jpeg_param_in->quant_luma[i+3];
			YQTable_hamac[32+j] = Gps_vec_jpeg_param_in->quant_luma[i+4];
			YQTable_hamac[40+j] = Gps_vec_jpeg_param_in->quant_luma[i+5];
			YQTable_hamac[48+j] = Gps_vec_jpeg_param_in->quant_luma[i+6];
			YQTable_hamac[56+j] = Gps_vec_jpeg_param_in->quant_luma[i+7];
		}

	for(i=0,j=0;i<64;i=i+8,j++)
		{
			CQTable_hamac[0+j] = Gps_vec_jpeg_param_in->quant_chroma[i];
			CQTable_hamac[8+j] = Gps_vec_jpeg_param_in->quant_chroma[i+1];
			CQTable_hamac[16+j] = Gps_vec_jpeg_param_in->quant_chroma[i+2];
			CQTable_hamac[24+j] = Gps_vec_jpeg_param_in->quant_chroma[i+3];
			CQTable_hamac[32+j] = Gps_vec_jpeg_param_in->quant_chroma[i+4];
			CQTable_hamac[40+j] = Gps_vec_jpeg_param_in->quant_chroma[i+5];
			CQTable_hamac[48+j] = Gps_vec_jpeg_param_in->quant_chroma[i+6];
			CQTable_hamac[56+j] = Gps_vec_jpeg_param_in->quant_chroma[i+7];
		}
}

/**************************************************************************
*
*  jpegenc_compress
*
*  This is the main function that encode the image
*
*  Input:
*        jpeg_p     : is the structure with all the parameters
*		  img        : is the image buffer
*        file_out   : is the output file name
*        file_out_dc: is the output file name for DC coefficients
*
*  Output:
*        return 0 if an error occurs;
*               the number of bits required to encode the image
*
*************************************************************************/

t_uint16 jpegenc_compress(t_uint32 addr_session_buffer)
{
	t_uint16 errorOrInfo;
	tps_t1xhv_vec_jpeg_session Gps_vec_jpeg_session = (tps_t1xhv_vec_jpeg_session)addr_session_buffer;

    /*memory requirements:
        dc_events: 1 events for each colour components and for each block (e.g. CIF case: (352/8+288/8)*3=4752 events;
        ac_events: up to 64 events for each colour components and for each block (e.g. CIF case: (352/8+288/8)*3*64=304128 events;
    */
#ifdef TRACE_ENABLE
	errorOrInfo = jpegenc_check_error(Gps_vec_jpeg_session);

	if(errorOrInfo != VEC_ERT_NONE)
	{
		return errorOrInfo;
	}

#endif

	/*inizialize data*/
	jpegenc_init_info(Gps_vec_jpeg_session);

	/*encode frame*/
	errorOrInfo = jpegenc_encode_frame(Gps_vec_jpeg_session);

	return errorOrInfo;
}


/**************************************************************************
*
*  jpeg_write_headers
*
*  This function write the headers to the file
*
*************************************************************************/
void jpegenc_write_headers(tps_t1xhv_vec_jpeg_session Gps_vec_jpeg_session)
{
	t_uint8 * p_hdr_buf;
    t_uint32 header_size = 0;
	tps_t1xhv_vec_header_buf Gps_jpeg_bitstream_buf_header = Gps_vec_jpeg_session->Gps_jpeg_bitstream_buf_header;

    p_hdr_buf = (t_uint8 *)Gps_jpeg_bitstream_buf_header->addr_header_buffer;
    header_size = Gps_jpeg_bitstream_buf_header->header_size;


    /*************************************************************************/
    /**
	* Copy from Header through MTF, written to bitstream using BPU, as in
	* t1xhv_m4e.c
	* Possible improvement: read from MTF and write to BPU in one loop?
	*/
    /*************************************************************************/

    if (header_size != 0)
    {
		jpegenc_write_n_bytes(header_size,p_hdr_buf, &Gps_vec_jpeg_session->Gs_t1xhv_vec_jpeg_global);
    }
}

/**************************************************************************
*
*  jpeg_encode_frame
*
*  This function encode the frame
*
*************************************************************************/

t_uint16 jpegenc_encode_frame(tps_t1xhv_vec_jpeg_session Gps_vec_jpeg_session)
{
	/*developed for MCU-units encoding*/
	t_sint32 m;
	t_sint32 i;
	t_sint32 N_restart_count;
	t_sint32 NBlocks;
	t_uint16 errorOrInfo;
	t_uint16 restart_width;
	t_uint32 SubSampFormat, WINDOW_WIDTH, WINDOW_HEIGHT;
	t_uint16 mblk_column, mblk_line;
	tps_t1xhv_vec_jpeg_param_in     Gps_vec_jpeg_param_in		 = Gps_vec_jpeg_session->Gps_vec_jpeg_param_in;
	tps_t1xhv_vec_jpeg_param_out    Gps_vec_jpeg_param_out		 = Gps_vec_jpeg_session->Gps_vec_jpeg_param_out;
	tps_t1xhv_vec_jpeg_param_inout  Gps_vec_jpeg_param_inout	 = Gps_vec_jpeg_session->Gps_vec_jpeg_param_inout;
	tps_t1xhv_vec_jpeg_param_inout  Gps_vec_jpeg_param_inout_out = Gps_vec_jpeg_session->Gps_vec_jpeg_param_inout_out;
	tps_t1xhv_vec_jpeg_global       Gps_t1xhv_vec_jpeg_global	 = &Gps_vec_jpeg_session->Gs_t1xhv_vec_jpeg_global;

	restart_width = Gps_vec_jpeg_param_in->restart_interval;

    Gps_t1xhv_vec_jpeg_global->DC_Prec_Y = Gps_vec_jpeg_param_inout->dc_predictor_y;
    Gps_t1xhv_vec_jpeg_global->DC_Prec_Cb = Gps_vec_jpeg_param_inout->dc_predictor_cb;
    Gps_t1xhv_vec_jpeg_global->DC_Prec_Cr = Gps_vec_jpeg_param_inout->dc_predictor_cr;
    N_restart_count = Gps_vec_jpeg_param_inout->restart_mcu_count;
    m = Gps_vec_jpeg_param_inout->restart_marker_id;

	SubSampFormat	= Gps_vec_jpeg_param_in->sampling_mode;
	WINDOW_WIDTH	= Gps_vec_jpeg_param_in->window_width;
	WINDOW_HEIGHT	= Gps_vec_jpeg_param_in->window_height;

    /*write the SOF and the header data*/
	jpegenc_write_headers(Gps_vec_jpeg_session);

	if(Gps_vec_jpeg_param_in->rotation)
	{
		t_uint16 x_mecc, y_mecc;
		//t_uint16 no_of_mcu;

		switch(SubSampFormat)
		{
		case MONOCHROME:
			mblk_line		= WINDOW_WIDTH/8;
			mblk_column		= WINDOW_HEIGHT/8;
			break;
		case FORMAT444:
			mblk_line		= WINDOW_WIDTH/8;
			mblk_column		= WINDOW_HEIGHT/8;
			break;
		case FORMAT420:
			mblk_line		= WINDOW_WIDTH/16;
			mblk_column		= WINDOW_HEIGHT/16;
			break;
		default:
			mblk_line		= WINDOW_WIDTH/8;
			mblk_column		= WINDOW_HEIGHT/16;
			break;
		}

		//no_of_mcu = mblk_column * mblk_line;

		x_mecc = 0;
		y_mecc = 0;


	    /* block fetching order change for +90 and -90 degree rotation
		==== ====									==== ====
		| 0 |  1 |        	     | 1 | 3 |
		==== ====	    +90->      ==== ====
		| 2 |  3 |	         	 | 0 | 2 |
		==== ====								==== ====
		Simlarly rotate in other direction for -90 degree rotation */

		for (;y_mecc<mblk_line;y_mecc++)
		{
			for (;x_mecc<mblk_column;x_mecc++)
			{

				if(restart_width)
				{
					if(N_restart_count==0)
					{
						/*append the restart marker*/
						jpegenc_byte_align(Gps_t1xhv_vec_jpeg_global);
						jpegenc_write_RST(m,Gps_t1xhv_vec_jpeg_global);
						m=(m+1)&7;
						N_restart_count=restart_width;
						jpegenc_reset_data(Gps_t1xhv_vec_jpeg_global);
					};
					N_restart_count-=1;
				}

				errorOrInfo =  jpegenc_encode_MCU_rotation(x_mecc,y_mecc,Gps_vec_jpeg_session);

				if(errorOrInfo==FALSE)
				{
					Gps_vec_jpeg_param_out->bitstream_size = 0;
					Gps_vec_jpeg_param_out->error_type = VEC_ERT_FATAL_ERROR;
					return VEC_ERT_FATAL_ERROR;
				}
			}
			x_mecc = 0;
		}
	}
	else
	{
		switch(SubSampFormat)
		{
		case MONOCHROME:
			NBlocks=WINDOW_WIDTH*WINDOW_HEIGHT/8/8;
			break;
		case FORMAT444:
			NBlocks=WINDOW_WIDTH*WINDOW_HEIGHT/8/8;
			break;
		case FORMAT420:
			NBlocks=WINDOW_WIDTH*WINDOW_HEIGHT/16/16;
			break;
		default:
			NBlocks=WINDOW_WIDTH*WINDOW_HEIGHT/16/8;
			break;
		}
		/*collects the events and the statistics*/
		for (i=0;i<NBlocks;i++)
		{
	#ifdef TRACE_ENABLE
			//printf("\n i = %d, Nbloack = %d\n",i,NBlocks);
	#endif
			if(restart_width)
			{
				if(N_restart_count==0)
				{
	#ifdef TRACE_ENABLE
					//printf("RESTART m=%d block number =%d\n", m, i);
	#endif
					/*append the restart marker*/
					jpegenc_byte_align(Gps_t1xhv_vec_jpeg_global);
					jpegenc_write_RST(m,Gps_t1xhv_vec_jpeg_global);
					m=(m+1)&7;
					N_restart_count=restart_width;
					jpegenc_reset_data(Gps_t1xhv_vec_jpeg_global);
				};
				N_restart_count-=1;
			}

			errorOrInfo = jpegenc_encode_MCU(i,Gps_vec_jpeg_session);

			if(errorOrInfo==FALSE)
			{
				Gps_vec_jpeg_param_out->bitstream_size = 0;
				Gps_vec_jpeg_param_out->error_type = VEC_ERT_FATAL_ERROR;
				return VEC_ERT_FATAL_ERROR;
			}
		}; //for (i=0;i<NBlocks;i++)
	} //if(Gps_vec_jpeg_param_in->rotation)

    /*write the End Of Image marker*/
	if(Gps_vec_jpeg_param_in->last_slice)
	{
		jpegenc_byte_align(Gps_t1xhv_vec_jpeg_global);
		jpegenc_write_EOI(Gps_t1xhv_vec_jpeg_global);
	}

	if(Gps_t1xhv_vec_jpeg_global->jpg_put_bits)
	{
		Gps_t1xhv_vec_jpeg_global->output_bytes[Gps_t1xhv_vec_jpeg_global->next_output_byte] =   ((Gps_t1xhv_vec_jpeg_global->jpg_put_buffer >> 16) & 0xFF);
	}

	Gps_vec_jpeg_param_inout_out->dc_predictor_y  = Gps_t1xhv_vec_jpeg_global->DC_Prec_Y;
    Gps_vec_jpeg_param_inout_out->dc_predictor_cb = Gps_t1xhv_vec_jpeg_global->DC_Prec_Cb;
    Gps_vec_jpeg_param_inout_out->dc_predictor_cr = Gps_t1xhv_vec_jpeg_global->DC_Prec_Cr;
    Gps_vec_jpeg_param_inout_out->restart_mcu_count = N_restart_count;
    Gps_vec_jpeg_param_inout_out->restart_marker_id = m;

	Gps_vec_jpeg_param_out->bitstream_size = (Gps_t1xhv_vec_jpeg_global->nTotalBits + Gps_t1xhv_vec_jpeg_global->jpg_put_bits - Gps_t1xhv_vec_jpeg_global->nStartBits);

	return VEC_ERT_NONE;
}

/**************************************************************************
*
*  jpeg_write_EOI
*
*  This function write the EOI marker
*
*************************************************************************/

void jpegenc_write_EOI(tps_t1xhv_vec_jpeg_global      Gps_t1xhv_vec_jpeg_global)
{
	jpegenc_emit_byte(0xFF, Gps_t1xhv_vec_jpeg_global);
	jpegenc_emit_byte(0xD9, Gps_t1xhv_vec_jpeg_global);
}


/**************************************************************************
*
*  jpeg_write_RST
*
*  This function write the restart interval in the stream
*
*************************************************************************/

void jpegenc_write_RST(t_sint32 m, tps_t1xhv_vec_jpeg_global      Gps_t1xhv_vec_jpeg_global)
{
	jpegenc_emit_byte(0xFF, Gps_t1xhv_vec_jpeg_global);
	jpegenc_emit_byte((t_uint8) (0xD0 | (t_uint8) m), Gps_t1xhv_vec_jpeg_global);
}

/**************************************************************************
*
*  jpeg_reset_data
*
*  This function reset the DCPred data for all component in the scan
*
*************************************************************************/

void jpegenc_reset_data(tps_t1xhv_vec_jpeg_global      Gps_t1xhv_vec_jpeg_global)
{
	Gps_t1xhv_vec_jpeg_global->DC_Prec_Y=0;
	Gps_t1xhv_vec_jpeg_global->DC_Prec_Cr=0;
	Gps_t1xhv_vec_jpeg_global->DC_Prec_Cb=0;
}

t_sint32 jpegenc_encode_MCU_rotation(t_uint16 x_mecc, t_uint16 y_mecc, tps_t1xhv_vec_jpeg_session Gps_vec_jpeg_session)
{
	t_uint16 errorOrInfo;
	//t_uint8 *position_in, *position_in_cr, *position_in_cb;
	//t_sint32 mcu_per_row;
	t_sint32 temp0, temp1, temp2, temp3, tempC;
    t_sint32 Umcu_h, Umcu_w;//, VSCALE;
	t_uint32 Uwidth, SubSampFormat, IMG_WIDTH, UV_OFFSET, MCU_WIDTH, MCU_HEIGHT,WINDOW_WIDTH, WINDOW_HEIGHT ;
	t_uint16 mblk_column, mblk_line;
	t_uint8 *jpeg_input_frame, *jpeg_input_frame_cr, *jpeg_input_frame_cb;
//	t_uint32 DChufftable, AChufftable;
	t_uint16 *YQTable_hamac , *CQTable_hamac;
	tps_t1xhv_vec_jpeg_param_in    Gps_vec_jpeg_param_in = Gps_vec_jpeg_session->Gps_vec_jpeg_param_in;
	tps_t1xhv_vec_frame_buf_in     Gps_vec_jpeg_frame_buf_in = Gps_vec_jpeg_session->Gps_vec_jpeg_frame_buf_in ;
	tps_t1xhv_vec_jpeg_global       Gps_t1xhv_vec_jpeg_global	 = &Gps_vec_jpeg_session->Gs_t1xhv_vec_jpeg_global;

	IMG_WIDTH = Gps_vec_jpeg_param_in->frame_width;
	UV_OFFSET = Gps_vec_jpeg_param_in->frame_width * Gps_vec_jpeg_param_in->frame_height;

	WINDOW_WIDTH	= Gps_vec_jpeg_param_in->window_width;
	WINDOW_HEIGHT	= Gps_vec_jpeg_param_in->window_height;

	MCU_WIDTH = Gps_t1xhv_vec_jpeg_global->MCU_WIDTH;
	MCU_HEIGHT = Gps_t1xhv_vec_jpeg_global->MCU_HEIGHT;

	YQTable_hamac = Gps_t1xhv_vec_jpeg_global->YQTable_hamac;
	CQTable_hamac = Gps_t1xhv_vec_jpeg_global->CQTable_hamac;

	jpeg_input_frame =  (t_uint8 *)(Gps_vec_jpeg_frame_buf_in->addr_source_buffer);
	jpeg_input_frame_cr =  jpeg_input_frame + UV_OFFSET ;

	jpeg_input_frame +=  (IMG_WIDTH * Gps_vec_jpeg_param_in->window_vertical_offset + Gps_vec_jpeg_param_in->window_horizontal_offset);

#ifdef TRACE_ENABLE
	printf("\n Function jpeg_encode_MCU_rotation: \n");
	printf("\n x_mecc %x, y_mecc %x \n", x_mecc,y_mecc);
	printf("\n mblk_line %x, mblk_column %x \n", mblk_line,mblk_column);
	printf("\n jpeg_input_frame: %x\n", jpeg_input_frame);
	printf("\n jpeg_input_frame_cr: %x\n", jpeg_input_frame_cr);
	printf("\n jpeg_input_frame_cb: %x\n", jpeg_input_frame_cb);
#endif

	SubSampFormat = Gps_vec_jpeg_param_in->sampling_mode;

	if (SubSampFormat == MONOCHROME)
	{
        Uwidth = IMG_WIDTH;
        Umcu_h = MCU_HEIGHT;
        Umcu_w = MCU_WIDTH;
		mblk_line		= WINDOW_WIDTH/8;
		mblk_column		= WINDOW_HEIGHT/8;

		if (Gps_vec_jpeg_param_in->rotation == 1)
		{
			temp0 = (x_mecc * IMG_WIDTH * MCU_HEIGHT) + ((mblk_line - y_mecc - 1) * MCU_WIDTH);
		}
		else
		{
			temp0 = ((mblk_column - x_mecc - 1) * IMG_WIDTH * MCU_HEIGHT) + (y_mecc * MCU_WIDTH);
		}

#ifdef TRACE_ENABLE
		printf("\n temp0=%x\n", temp0);
#endif

		Gps_t1xhv_vec_jpeg_global->DChufftable = (t_uint32)&jpegenc_Y_DC_hufftable;
		Gps_t1xhv_vec_jpeg_global->AChufftable = (t_uint32)&jpegenc_Y_AC_hufftable;

		errorOrInfo = jpegenc_encode_block(jpeg_input_frame + temp0,&Gps_t1xhv_vec_jpeg_global->DC_Prec_Y,IMG_WIDTH, YQTable_hamac,0,Gps_vec_jpeg_session);

		if(errorOrInfo==FALSE)
		{
			return FALSE;
		}
	}  //if (SubSampFormat == MONOCHROME)
	else if (SubSampFormat == FORMAT420)
	{
		//VSCALE = 2;
        Uwidth = IMG_WIDTH>>1;
        Umcu_h = MCU_HEIGHT>>1;
        Umcu_w = MCU_WIDTH>>1;
		mblk_line		= WINDOW_WIDTH/16;
		mblk_column		= WINDOW_HEIGHT/16;

		jpeg_input_frame_cb =  jpeg_input_frame_cr + (UV_OFFSET>>2) ;
		jpeg_input_frame_cr +=  ((IMG_WIDTH/2) * (Gps_vec_jpeg_param_in->window_vertical_offset/2) + (Gps_vec_jpeg_param_in->window_horizontal_offset/2));
		jpeg_input_frame_cb +=  ((IMG_WIDTH/2) * (Gps_vec_jpeg_param_in->window_vertical_offset/2) + (Gps_vec_jpeg_param_in->window_horizontal_offset/2));

		Gps_t1xhv_vec_jpeg_global->DChufftable = (t_uint32)&jpegenc_Y_DC_hufftable;
		Gps_t1xhv_vec_jpeg_global->AChufftable = (t_uint32)&jpegenc_Y_AC_hufftable;

		if (Gps_vec_jpeg_param_in->rotation == 1)
		{
			temp0 =  (x_mecc * IMG_WIDTH * MCU_HEIGHT) +  ((mblk_line - y_mecc - 1) * MCU_WIDTH + (MCU_WIDTH>>1));
			temp1 =  ((x_mecc * IMG_WIDTH * MCU_HEIGHT) + ((MCU_HEIGHT>>1) * IMG_WIDTH) ) + ((mblk_line - y_mecc - 1) * MCU_WIDTH + (MCU_WIDTH>>1)) ;
			temp2 = (x_mecc * IMG_WIDTH * MCU_HEIGHT) + ((mblk_line - y_mecc - 1) * MCU_WIDTH);
			temp3 =  ((x_mecc * IMG_WIDTH * MCU_HEIGHT) + ((MCU_HEIGHT>>1) * IMG_WIDTH))  + ((mblk_line - y_mecc - 1) * MCU_WIDTH) ;
			tempC =  (x_mecc * Uwidth * Umcu_h) +  ((mblk_line - y_mecc - 1) * Umcu_w);
		}
		else
		{
			temp0 = ((mblk_column - x_mecc - 1)  * IMG_WIDTH * MCU_HEIGHT  + ((MCU_HEIGHT>>1) * IMG_WIDTH)) +   (y_mecc * MCU_WIDTH) ;
			temp1 = ((mblk_column - x_mecc - 1) * IMG_WIDTH * MCU_HEIGHT) + (y_mecc * MCU_WIDTH);
			temp2 =  (((mblk_column - x_mecc - 1)  * IMG_WIDTH * MCU_HEIGHT) + ((MCU_HEIGHT>>1) * IMG_WIDTH))  +  (y_mecc * MCU_WIDTH + (MCU_WIDTH>>1)) ;
			temp3 =  ((mblk_column - x_mecc - 1) * IMG_WIDTH * MCU_HEIGHT) +  (y_mecc * MCU_WIDTH + (MCU_WIDTH>>1));
			tempC =  ((mblk_column - x_mecc - 1) * Uwidth * Umcu_h) +  (y_mecc * Umcu_w);
		}

#ifdef TRACE_ENABLE
		printf("\n jpeg_input_frame_cb: %x\n", jpeg_input_frame_cb);
		printf("\n temp0=%x\n", temp0);
		printf("\n temp1=%x\n", temp1);
		printf("\n temp2=%x\n", temp2);
		printf("\n temp3=%x\n", temp3);
		printf("\n tempC=%x\n", tempC);
#endif
		//encode Y0
		errorOrInfo = jpegenc_encode_block(jpeg_input_frame + temp0,&Gps_t1xhv_vec_jpeg_global->DC_Prec_Y,IMG_WIDTH, YQTable_hamac,0,Gps_vec_jpeg_session);

		if(errorOrInfo==FALSE)
		{
			return FALSE;
		}

		//encode Y1
		errorOrInfo = jpegenc_encode_block(jpeg_input_frame + temp1,&Gps_t1xhv_vec_jpeg_global->DC_Prec_Y,IMG_WIDTH,YQTable_hamac,0,Gps_vec_jpeg_session);

		if(errorOrInfo==FALSE)
		{
			return FALSE;
		}

		//encode Y2
		errorOrInfo = jpegenc_encode_block(jpeg_input_frame + temp2,&Gps_t1xhv_vec_jpeg_global->DC_Prec_Y,IMG_WIDTH, YQTable_hamac,0,Gps_vec_jpeg_session);

		if(errorOrInfo==FALSE)
		{
			return FALSE;
		}

		//encode Y3
		errorOrInfo = jpegenc_encode_block(jpeg_input_frame + temp3,&Gps_t1xhv_vec_jpeg_global->DC_Prec_Y,IMG_WIDTH,YQTable_hamac,0,Gps_vec_jpeg_session);

		if(errorOrInfo==FALSE)
		{
			return FALSE;
		}

		//encode Cr
		//DChufftable = (t_uint32)&jpegenc_C_DC_hufftable;
		//AChufftable = (t_uint32)&jpegenc_C_AC_hufftable;

		errorOrInfo = jpegenc_encode_block(jpeg_input_frame_cr + tempC,&Gps_t1xhv_vec_jpeg_global->DC_Prec_Cr,Uwidth,CQTable_hamac,0,Gps_vec_jpeg_session);

		if(errorOrInfo==FALSE)
		{
			return FALSE;
		}

		//encode Cb
		errorOrInfo = jpegenc_encode_block(jpeg_input_frame_cb + tempC,&Gps_t1xhv_vec_jpeg_global->DC_Prec_Cb,Uwidth,CQTable_hamac,0,Gps_vec_jpeg_session);

		if(errorOrInfo==FALSE)
		{
			return FALSE;
		}
	}
	else if (SubSampFormat == FORMAT422)
	{
        //VSCALE = 1;
        Uwidth = IMG_WIDTH>>1;
        Umcu_h = MCU_HEIGHT;
        Umcu_w = MCU_WIDTH>>1;
		mblk_line		= WINDOW_WIDTH/8;
		mblk_column		= WINDOW_HEIGHT/16;

		jpeg_input_frame_cb =  jpeg_input_frame_cr + (UV_OFFSET>>1) ;
		jpeg_input_frame_cr +=  ((IMG_WIDTH/2) * Gps_vec_jpeg_param_in->window_vertical_offset + (Gps_vec_jpeg_param_in->window_horizontal_offset/2));
		jpeg_input_frame_cb +=  ((IMG_WIDTH/2) * Gps_vec_jpeg_param_in->window_vertical_offset + (Gps_vec_jpeg_param_in->window_horizontal_offset/2));

		Gps_t1xhv_vec_jpeg_global->DChufftable = (t_uint32)&jpegenc_Y_DC_hufftable;
		Gps_t1xhv_vec_jpeg_global->AChufftable = (t_uint32)&jpegenc_Y_AC_hufftable;

		if (Gps_vec_jpeg_param_in->rotation == 1)
		{
			temp0 =  (x_mecc * IMG_WIDTH * MCU_HEIGHT) +  ((mblk_line - y_mecc - 1) * MCU_WIDTH + (MCU_WIDTH>>1));
			temp1 =  ((x_mecc * IMG_WIDTH * MCU_HEIGHT) + ((MCU_HEIGHT>>1) * IMG_WIDTH))  + ((mblk_line - y_mecc - 1) * MCU_WIDTH) ;
			tempC =  (x_mecc * Uwidth * Umcu_h) +  ((mblk_line - y_mecc - 1) * Umcu_w);
		}
		else
		{
			temp0 = ((mblk_column - x_mecc - 1)  * IMG_WIDTH * MCU_HEIGHT  + ((MCU_HEIGHT>>1) * IMG_WIDTH)) +   (y_mecc * MCU_WIDTH) ;
			temp1 = ((mblk_column - x_mecc - 1) * IMG_WIDTH * MCU_HEIGHT) + (y_mecc * MCU_WIDTH);
			tempC =  ((mblk_column - x_mecc - 1) * Uwidth * Umcu_h) +  (y_mecc * Umcu_w);
		}

#ifdef TRACE_ENABLE
		printf("\n temp0=%x\n", temp0);
		printf("\n temp1=%x\n", temp1);
		printf("\n tempC=%x\n", tempC);
#endif

		//encode Y0
		errorOrInfo = jpegenc_encode_block(jpeg_input_frame + temp0,&Gps_t1xhv_vec_jpeg_global->DC_Prec_Y,IMG_WIDTH, YQTable_hamac,0,Gps_vec_jpeg_session);

		if(errorOrInfo==FALSE)
		{
			return FALSE;
		}

		//encode Y1
		errorOrInfo = jpegenc_encode_block(jpeg_input_frame + temp1,&Gps_t1xhv_vec_jpeg_global->DC_Prec_Y,IMG_WIDTH,YQTable_hamac,0,Gps_vec_jpeg_session);

		if(errorOrInfo==FALSE)
		{
			return FALSE;
		}

		//encode Cr
		//DChufftable = (t_uint32)&jpegenc_C_DC_hufftable;
		//AChufftable = (t_uint32)&jpegenc_C_AC_hufftable;

		errorOrInfo = jpegenc_encode_block(jpeg_input_frame_cr + tempC,&Gps_t1xhv_vec_jpeg_global->DC_Prec_Cr,Uwidth,CQTable_hamac,0,Gps_vec_jpeg_session);

		if(errorOrInfo==FALSE)
		{
			return FALSE;
		}

		//encode Cb
		errorOrInfo = jpegenc_encode_block(jpeg_input_frame_cb + tempC,&Gps_t1xhv_vec_jpeg_global->DC_Prec_Cb,Uwidth,CQTable_hamac,0,Gps_vec_jpeg_session);

		if(errorOrInfo==FALSE)
		{
			return FALSE;
		}
	}
	else  //FORMAT444
	{
        //VSCALE = 0;
        Uwidth = IMG_WIDTH;
        Umcu_h = MCU_HEIGHT;
        Umcu_w = MCU_WIDTH;
		mblk_line		= WINDOW_WIDTH/8;
		mblk_column		= WINDOW_HEIGHT/8;

		jpeg_input_frame_cb =  jpeg_input_frame_cr + UV_OFFSET ;
		jpeg_input_frame_cr +=  (IMG_WIDTH * Gps_vec_jpeg_param_in->window_vertical_offset + Gps_vec_jpeg_param_in->window_horizontal_offset);
		jpeg_input_frame_cb +=  (IMG_WIDTH * Gps_vec_jpeg_param_in->window_vertical_offset + Gps_vec_jpeg_param_in->window_horizontal_offset);

		Gps_t1xhv_vec_jpeg_global->DChufftable = (t_uint32)&jpegenc_Y_DC_hufftable;
		Gps_t1xhv_vec_jpeg_global->AChufftable = (t_uint32)&jpegenc_Y_AC_hufftable;

		if (Gps_vec_jpeg_param_in->rotation == 1)
		{
			temp0 = (x_mecc * IMG_WIDTH * MCU_HEIGHT) + ((mblk_line - y_mecc - 1) * MCU_WIDTH);
			tempC =  (x_mecc * Uwidth * Umcu_h) +  ((mblk_line - y_mecc - 1) * Umcu_w);
		}
		else
		{
			temp0 = ((mblk_column - x_mecc - 1) * IMG_WIDTH * MCU_HEIGHT) + (y_mecc * MCU_WIDTH);
			tempC =  ((mblk_column - x_mecc - 1) * Uwidth * Umcu_h) +  (y_mecc * Umcu_w);
		}

#ifdef TRACE_ENABLE
		printf("\n temp0=%x\n", temp0);
		printf("\n tempC=%x\n", tempC);
#endif

		//encode Y0
		errorOrInfo = jpegenc_encode_block(jpeg_input_frame + temp0,&Gps_t1xhv_vec_jpeg_global->DC_Prec_Y,IMG_WIDTH, YQTable_hamac,0,Gps_vec_jpeg_session);

		if(errorOrInfo==FALSE)
		{
			return FALSE;
		}

		//encode Cr
		//DChufftable = (t_uint32)&jpegenc_C_DC_hufftable;
		//AChufftable = (t_uint32)&jpegenc_C_AC_hufftable;

		errorOrInfo = jpegenc_encode_block(jpeg_input_frame_cr + tempC,&Gps_t1xhv_vec_jpeg_global->DC_Prec_Cr,Uwidth,CQTable_hamac,0,Gps_vec_jpeg_session);

		if(errorOrInfo==FALSE)
		{
			return FALSE;
		}

		//encode Cb
		errorOrInfo = jpegenc_encode_block(jpeg_input_frame_cb + tempC,&Gps_t1xhv_vec_jpeg_global->DC_Prec_Cb,Uwidth,CQTable_hamac,0,Gps_vec_jpeg_session);

		if(errorOrInfo==FALSE)
		{
			return FALSE;
		}
	} //if (SubSampFormat == MONOCHROME)

	return 	TRUE;
}
/**************************************************************************
*
*  jpegenc_encode_MCU
*
*  This function encode one MCU collecting all the stats and the events
*
*************************************************************************/

t_sint32 jpegenc_encode_MCU(t_sint32 i, tps_t1xhv_vec_jpeg_session Gps_vec_jpeg_session)
{
	t_uint16 errorOrInfo;
	t_uint8 *position_in, *position_in_cr, *position_in_cb;
	t_sint32 mcu_per_row;
	t_sint32 temp;
    t_sint32 Umcu_h, Umcu_w;//, VSCALE;
	t_uint32 Uwidth, SubSampFormat,IMG_WIDTH, WINDOW_WIDTH, UV_OFFSET,MCU_WIDTH, MCU_HEIGHT;
	t_uint8 *jpeg_input_frame, *jpeg_input_frame_cr, *jpeg_input_frame_cb = 0x0;
	t_uint16 *YQTable_hamac , *CQTable_hamac;
	tps_t1xhv_vec_jpeg_param_in    Gps_vec_jpeg_param_in = Gps_vec_jpeg_session->Gps_vec_jpeg_param_in;
	tps_t1xhv_vec_frame_buf_in     Gps_vec_jpeg_frame_buf_in = Gps_vec_jpeg_session->Gps_vec_jpeg_frame_buf_in ;
	tps_t1xhv_vec_jpeg_global       Gps_t1xhv_vec_jpeg_global	 = &Gps_vec_jpeg_session->Gs_t1xhv_vec_jpeg_global;

	IMG_WIDTH		= Gps_vec_jpeg_param_in->frame_width;
	WINDOW_WIDTH	= Gps_vec_jpeg_param_in->window_width;
	//WINDOW_HEIGHT	= Gps_vec_jpeg_param_in->window_height;
	UV_OFFSET       = Gps_vec_jpeg_param_in->frame_width * Gps_vec_jpeg_param_in->frame_height;

	MCU_WIDTH = Gps_t1xhv_vec_jpeg_global->MCU_WIDTH;
	MCU_HEIGHT = Gps_t1xhv_vec_jpeg_global->MCU_HEIGHT;

	YQTable_hamac = Gps_t1xhv_vec_jpeg_global->YQTable_hamac;
	CQTable_hamac = Gps_t1xhv_vec_jpeg_global->CQTable_hamac;

	jpeg_input_frame =  (t_uint8 *)(Gps_vec_jpeg_frame_buf_in->addr_source_buffer);
	jpeg_input_frame_cr =  jpeg_input_frame + UV_OFFSET ;

	jpeg_input_frame +=  (IMG_WIDTH * Gps_vec_jpeg_param_in->window_vertical_offset + Gps_vec_jpeg_param_in->window_horizontal_offset);

	mcu_per_row=WINDOW_WIDTH/MCU_WIDTH;

	SubSampFormat = Gps_vec_jpeg_param_in->sampling_mode;

	//fprintf(huff_file,"SubSampFormat = %d  \n",SubSampFormat);
    switch(SubSampFormat)
    {
	case MONOCHROME:
        Uwidth = IMG_WIDTH;
        Umcu_h = MCU_HEIGHT;
        Umcu_w = MCU_WIDTH;
        break;
    case FORMAT444:
        //VSCALE = 0;
        Uwidth = IMG_WIDTH;
        Umcu_h = MCU_HEIGHT;
        Umcu_w = MCU_WIDTH;
		jpeg_input_frame_cb =  jpeg_input_frame_cr + UV_OFFSET ;
		jpeg_input_frame_cr +=  (IMG_WIDTH * Gps_vec_jpeg_param_in->window_vertical_offset + Gps_vec_jpeg_param_in->window_horizontal_offset);
		jpeg_input_frame_cb +=  (IMG_WIDTH * Gps_vec_jpeg_param_in->window_vertical_offset + Gps_vec_jpeg_param_in->window_horizontal_offset);
        break;
    case FORMAT420:
        //VSCALE = 2;
        Uwidth = IMG_WIDTH>>1;
        Umcu_h = MCU_HEIGHT>>1;
        Umcu_w = MCU_WIDTH>>1;
		jpeg_input_frame_cb =  jpeg_input_frame_cr + (UV_OFFSET>>2) ;
		jpeg_input_frame_cr +=  ((IMG_WIDTH/2) * (Gps_vec_jpeg_param_in->window_vertical_offset/2) + (Gps_vec_jpeg_param_in->window_horizontal_offset/2));
		jpeg_input_frame_cb +=  ((IMG_WIDTH/2) * (Gps_vec_jpeg_param_in->window_vertical_offset/2) + (Gps_vec_jpeg_param_in->window_horizontal_offset/2));
        break;
    default: //default 422 planar
        Uwidth = IMG_WIDTH>>1;
        Umcu_h = MCU_HEIGHT;
        Umcu_w = MCU_WIDTH>>1;
		if (SubSampFormat==FORMAT422INT)
		{
			jpeg_input_frame_cb =  jpeg_input_frame;
			jpeg_input_frame_cr =  jpeg_input_frame;
			jpeg_input_frame_cb =  jpeg_input_frame;
		}
		else
		{
			//VSCALE = 1;
			jpeg_input_frame_cb =  jpeg_input_frame_cr + (UV_OFFSET>>1) ;
			jpeg_input_frame_cr +=  ((IMG_WIDTH/2) * Gps_vec_jpeg_param_in->window_vertical_offset + (Gps_vec_jpeg_param_in->window_horizontal_offset/2));
			jpeg_input_frame_cb +=  ((IMG_WIDTH/2) * Gps_vec_jpeg_param_in->window_vertical_offset + (Gps_vec_jpeg_param_in->window_horizontal_offset/2));
		}
        break;
    }

	// fprintf(huff_file,"i = %d IMG_WIDTH = %d MCU_WIDTH = %d  \n",i,IMG_WIDTH, MCU_WIDTH);
	// fprintf(huff_file,"Uwidth = %d VSCALE = %d Umcu_h = %d Umcu_w %d \n",Uwidth,VSCALE, Umcu_h,Umcu_w);


	if (SubSampFormat == MONOCHROME)
	{
		temp=(i/mcu_per_row)*IMG_WIDTH*MCU_HEIGHT+(i%mcu_per_row)*MCU_WIDTH;
		position_in	= jpeg_input_frame + temp;

		Gps_t1xhv_vec_jpeg_global->DChufftable = (t_uint32)&jpegenc_Y_DC_hufftable;
		Gps_t1xhv_vec_jpeg_global->AChufftable = (t_uint32)&jpegenc_Y_AC_hufftable;

		errorOrInfo = jpegenc_encode_block(position_in,&Gps_t1xhv_vec_jpeg_global->DC_Prec_Y,IMG_WIDTH, YQTable_hamac,0,Gps_vec_jpeg_session);

		if(errorOrInfo==FALSE)
		{
			return FALSE;
		}
	}  //if (SubSampFormat == MONOCHROME)
	else
	{
		Gps_t1xhv_vec_jpeg_global->DChufftable = (t_uint32)&jpegenc_Y_DC_hufftable;
		Gps_t1xhv_vec_jpeg_global->AChufftable = (t_uint32)&jpegenc_Y_AC_hufftable;
		//encode Y1 only
		if (SubSampFormat==FORMAT422INT)
		{
			temp=(i/mcu_per_row)*IMG_WIDTH*MCU_HEIGHT*2+(i%mcu_per_row)*MCU_WIDTH*2; //calculation for Y
		}
		else
		{
			temp=(i/mcu_per_row)*IMG_WIDTH*MCU_HEIGHT+(i%mcu_per_row)*MCU_WIDTH;
		}

		position_in	= jpeg_input_frame + temp;

		//fprintf(huff_file,"position_in = %u  \n",temp);
		errorOrInfo = jpegenc_encode_block(position_in,&Gps_t1xhv_vec_jpeg_global->DC_Prec_Y,IMG_WIDTH, YQTable_hamac,0,Gps_vec_jpeg_session);

		if(errorOrInfo==FALSE)
		{
			return FALSE;
		}

		if(!(SubSampFormat==FORMAT444))
		{
			//encode Y2 only
			if (SubSampFormat==FORMAT422INT)
			{
				errorOrInfo = jpegenc_encode_block(position_in+(MCU_WIDTH),&Gps_t1xhv_vec_jpeg_global->DC_Prec_Y,IMG_WIDTH,YQTable_hamac,0,Gps_vec_jpeg_session);
			}
			else
			{
				errorOrInfo = jpegenc_encode_block(position_in+(MCU_WIDTH>>1),&Gps_t1xhv_vec_jpeg_global->DC_Prec_Y,IMG_WIDTH,YQTable_hamac,0,Gps_vec_jpeg_session);
			}

			if(errorOrInfo==FALSE)
			{
				return FALSE;
			}

			if(SubSampFormat==FORMAT420)
			{
				errorOrInfo = jpegenc_encode_block(position_in+IMG_WIDTH*(MCU_HEIGHT>>1),&Gps_t1xhv_vec_jpeg_global->DC_Prec_Y,IMG_WIDTH, YQTable_hamac,0,Gps_vec_jpeg_session);

				if(errorOrInfo==FALSE)
				{
					return FALSE;
				}

				errorOrInfo = jpegenc_encode_block(position_in+IMG_WIDTH*(MCU_HEIGHT>>1)+(MCU_WIDTH>>1),&Gps_t1xhv_vec_jpeg_global->DC_Prec_Y,IMG_WIDTH,YQTable_hamac,0,Gps_vec_jpeg_session);

				if(errorOrInfo==FALSE)
				{
					return FALSE;
				}
			} //if(SubSampFormat==FORMAT420)
		} //if(!(SubSampFormat==FORMAT444))
		//encode Cr, Cb

		Gps_t1xhv_vec_jpeg_global->DChufftable = (t_uint32)&jpegenc_C_DC_hufftable;
		Gps_t1xhv_vec_jpeg_global->AChufftable = (t_uint32)&jpegenc_C_AC_hufftable;

		if (SubSampFormat==FORMAT422INT)
		{
			position_in_cr	= jpeg_input_frame_cr + (i/mcu_per_row)*Uwidth*Umcu_h*4+(i%mcu_per_row)*Umcu_w*4;
			position_in_cb	= jpeg_input_frame_cb + (i/mcu_per_row)*Uwidth*Umcu_h*4+(i%mcu_per_row)*Umcu_w*4;
		}
		else
		{
			position_in_cr	= jpeg_input_frame_cr + (i/mcu_per_row)*Uwidth*Umcu_h+(i%mcu_per_row)*Umcu_w;
			position_in_cb	= jpeg_input_frame_cb + (i/mcu_per_row)*Uwidth*Umcu_h+(i%mcu_per_row)*Umcu_w;
		}
		errorOrInfo = jpegenc_encode_block(position_in_cr,&Gps_t1xhv_vec_jpeg_global->DC_Prec_Cr,Uwidth,CQTable_hamac,1,Gps_vec_jpeg_session);

		if(errorOrInfo==FALSE)
		{
			return FALSE;
		}

		errorOrInfo = jpegenc_encode_block(position_in_cb,&Gps_t1xhv_vec_jpeg_global->DC_Prec_Cb,Uwidth,CQTable_hamac,2,Gps_vec_jpeg_session);

		if(errorOrInfo==FALSE)
		{
			return FALSE;
		}
	} //if (SubSampFormat == MONOCHROME)
	return TRUE;
}

/**************************************************************************
*
*  jpegenc_byte_align
*
*  This function flush with 1 the last byte encoded
*
*************************************************************************/
void jpegenc_byte_align(tps_t1xhv_vec_jpeg_global      Gps_t1xhv_vec_jpeg_global)
{
	jpegenc_emit_bits(0x7F, 7, Gps_t1xhv_vec_jpeg_global); /* fill any partial byte with ones */
	Gps_t1xhv_vec_jpeg_global->jpg_put_buffer = 0;	/* and reset bit-buffer to empty */
	Gps_t1xhv_vec_jpeg_global->jpg_put_bits = 0;

};

/**************************************************************************
*
*  jpegenc_emit_byte
*
*  This function write the byte in the stream
*
*************************************************************************/
void jpegenc_emit_byte (t_uint32 val, tps_t1xhv_vec_jpeg_global      Gps_t1xhv_vec_jpeg_global)
/* Emit a byte */
{
	Gps_t1xhv_vec_jpeg_global->output_bytes[Gps_t1xhv_vec_jpeg_global->next_output_byte] = (t_uint8) (val & 0xff);

	Gps_t1xhv_vec_jpeg_global->next_output_byte++;

	if(Gps_t1xhv_vec_jpeg_global->next_output_byte >= Gps_t1xhv_vec_jpeg_global->bitstreamBufferSize)
	{
		Gps_t1xhv_vec_jpeg_global->next_output_byte = 0;
	}

	Gps_t1xhv_vec_jpeg_global->nTotalBits += 8;
}

/**************************************************************************
*
*  jpegenc_emit_bits
*
*  This function insert some bits in the stream.
*  It insert the 0 stuffing byte if the 0xFF byte is obtained.
*
*************************************************************************/
void jpegenc_emit_bits (t_uint32 code, t_sint32 size, tps_t1xhv_vec_jpeg_global      Gps_t1xhv_vec_jpeg_global)
/* Emit some bits; return TRUE if successful, FALSE if must suspend */
{
	/* This routine is heavily used, so it's worth coding tightly. */
	register t_sint32 put_buffer = (t_sint32) code;
	register t_sint32 put_bits = Gps_t1xhv_vec_jpeg_global->jpg_put_bits;


	put_buffer &= (((t_sint32) 1)<<size) - 1; /* mask off any extra bits in code */

	put_bits += size;		/* new number of bits in buffer */

	put_buffer <<= 24 - put_bits; /* align incoming bits */

	put_buffer |= Gps_t1xhv_vec_jpeg_global->jpg_put_buffer; /* and merge with old buffer contents */

	while (put_bits >= 8) {
		t_sint32 c = (t_sint32) ((put_buffer >> 16) & 0xFF);

		jpegenc_emit_byte((t_uint8)c, Gps_t1xhv_vec_jpeg_global);
		if (c == 0xFF) {		/* need to stuff a zero byte? */
			jpegenc_emit_byte(0, Gps_t1xhv_vec_jpeg_global);
		}
		put_buffer <<= 8;
		put_bits -= 8;
	}

	Gps_t1xhv_vec_jpeg_global->jpg_put_buffer = put_buffer; /* update state variables */
	Gps_t1xhv_vec_jpeg_global->jpg_put_bits = put_bits;
}

/**************************************************************************
*
*  jpegenc_rasterize_block
*
*  This function rasterizes a block of 64 values
*
*
*************************************************************************/
void jpegenc_rasterize_block(t_uint8 *position_in, t_sint16 *block, t_uint32 x_dim)
{
	t_uint32 i,j;

	for(i = 0; i < 8; i++)
	{
		for(j = 0; j < 8; j++)
		{
			block[8*i+j] = position_in[x_dim*i+j];
			/*fprintf(huff_file,"val %d i = %d j = %d  x_dim = %d\n",block[8*i+j],i,j,x_dim );              */
		}
	}
}

/**************************************************************************
*
*  jpegenc_rasterize_block_Y
*
*  This function rasterizes a block of 64 values
*
*
*************************************************************************/
void jpegenc_rasterize_block_422int_Y(t_uint8 *position_in, t_sint16 *block, t_uint32 x_dim)
{
	t_uint32 i,j;
	position_in = position_in +1; //for UYVY

	for(i = 0; i < 8; i++)
	{
		for(j = 0; j < 8; j++)
		{
			block[8*i+j] = position_in[x_dim*i+j*2];
			/*fprintf(huff_file,"val %d i = %d j = %d  x_dim = %d\n",block[8*i+j],i,j,x_dim );              */
		}
	}
}

/**************************************************************************
*
*  jpegenc_rasterize_block_Cb for U components
*
*  This function rasterizes a block of 64 values
*
*
*************************************************************************/
void jpegenc_rasterize_block_422int_Cb(t_uint8 *position_in, t_sint16 *block, t_uint32 x_dim)
{
	t_uint32 i,j;

	for(i = 0; i < 8; i++)
	{
		for(j = 0; j < 8; j++)
		{
			block[8*i+j] = position_in[x_dim*i+j*4];
			/*fprintf(huff_file,"val %d i = %d j = %d  x_dim = %d\n",block[8*i+j],i,j,x_dim );              */
		}
	}
}

/**************************************************************************
*
*  jpegenc_rasterize_block_Cr for V components
*
*  This function rasterizes a block of 64 values
*
*
*************************************************************************/
void jpegenc_rasterize_block_422int_Cr(t_uint8 *position_in, t_sint16 *block, t_uint32 x_dim)
{
	t_uint32 i,j;
	position_in = position_in +2;
	for(i = 0; i < 8; i++)
	{
		for(j = 0; j < 8; j++)
		{
			block[8*i+j] = position_in[x_dim*i+j*4];
			/*fprintf(huff_file,"val %d i = %d j = %d  x_dim = %d\n",block[8*i+j],i,j,x_dim );              */
		}
	}
}

/**************************************************************************
*
*  jpegenc_rasterize_block_rot1
*
*  This function rasterizes a block of 64 values with +90 rotation
*
*
*************************************************************************/
void jpegenc_rasterize_block_rot1(t_uint8 *position_in, t_sint16 *block, t_uint32 x_dim)
{
	t_uint32 i,j;

	for(i = 0; i < 8; i++)
	{
		for(j = 0; j < 8; j++)
		{
			block[8*i+j] = position_in[x_dim*j + 7 - i];
		}
	}
}

/**************************************************************************
*
*  jpegenc_rasterize_block_rot2
*
*  This function rasterizes a block of 64 values with -90 rotation
*
*
*************************************************************************/
void jpegenc_rasterize_block_rot2(t_uint8 *position_in, t_sint16 *block, t_uint32 x_dim)
{
	t_uint32 i,j;

	for(i = 0; i < 8; i++)
	{
		for(j = 0; j < 8; j++)
		{
			block[8*i+j] = position_in[(x_dim* (7-j)) + i];
		}
	}
}

/**************************************************************************
*
*  jpegenc_quant
*
*  Quantizer bit accurate C model
*
*
*************************************************************************/
t_sint32 jpegenc_quant (t_sint32 f, t_sint32 qp, t_sint32 r, t_sint32 s, t_sint32 t)
{
	t_sint32 sign_r, abs_r, f_adjusted ;

	sign_r = (r>>2)&1 ; /*....................... sign(r) = r[2]        */
						abs_r  = r&3  ; /*........................... abs(r) = r[1:0]
										r binary code | integer value
										--------------|--------------
										111 | -3
										110 | -2
										101 | -1
										000 |  0
										001 |  1
										010 |  2
						011 |  3    */
						if (sign_r) /*............................... r is negative         */
						{
							f_adjusted = f-((qp*abs_r)/4) ;
							if (f_adjusted<0) f_adjusted = 0; /*...... clip to 0             */
						}
						else /*...................................... r is positive or zero */
						{
							f_adjusted = f+((qp*abs_r)/4) ;
							if (f_adjusted>2047) f_adjusted = 2047; /* clip to 2047          */
						}
						return((((f_adjusted*2)/((s+1)*qp))+t)/2) ;
}


/**************************************************************************
*
*  jpegenc_encode_block
*
*  This function encode the block (Y.U.V) pointed by position_in
*  collecting all the stats and the events
*
*************************************************************************/
t_sint32 jpegenc_encode_block(t_uint8 *position_in,t_sint16 *Prec_DC, t_uint32 width, t_uint16 * qtable,t_uint8 compType, tps_t1xhv_vec_jpeg_session Gps_vec_jpeg_session)
{
	t_sint16 a[64];
	t_sint16 b[64];
	t_sint32 qval;
	t_sint32 sign;
	register t_sint32 temp, temp2;
	register t_sint32 nbits;
	register t_sint32 k, r, i;
	jpegenc_huffman_table *DChufftable;
	tps_t1xhv_vec_jpeg_param_in    Gps_vec_jpeg_param_in = Gps_vec_jpeg_session->Gps_vec_jpeg_param_in;
	tps_t1xhv_vec_jpeg_global       Gps_t1xhv_vec_jpeg_global	 = &Gps_vec_jpeg_session->Gs_t1xhv_vec_jpeg_global;

	DChufftable = (jpegenc_huffman_table *)Gps_t1xhv_vec_jpeg_global->DChufftable;

#ifdef TRACE_ENABLE
	printf("position_in = %x, rasterize_block_rot0\n", position_in);
	jpegenc_rasterize_block(position_in, b, width);

	for(i=0;i<64;i++)
	{
		printf("%d ",b[i]);

		if((i+1)%8 == 0)
			printf("\n");
	}

	printf("position_in = %x, rasterize_block_rot1\n", position_in);
	jpegenc_rasterize_block_rot1(position_in, b, width);

	printf("\n");
	for(i=0;i<64;i++)
	{
		printf("%d ",b[i]);

		if((i+1)%8 == 0)
			printf("\n");
	}

	printf("position_in = %x, rasterize_block_rot2\n", position_in);
	jpegenc_rasterize_block_rot2(position_in, b, width);

	printf("\n");
	for(i=0;i<64;i++)
	{
		printf("%d ",b[i]);

		if((i+1)%8 == 0)
			printf("\n");
	}
#else

	/*forward DCT*/
	if (Gps_vec_jpeg_param_in->rotation == 0)
	{
		if (Gps_vec_jpeg_param_in->sampling_mode==FORMAT422INT)
		{
			if(compType == 0)
			{
				jpegenc_rasterize_block_422int_Y(position_in, b, width*2);
			}
			else if (compType ==1)
			{
				jpegenc_rasterize_block_422int_Cb(position_in, b, width*4);
			}
			else if (compType ==2)
			{
				jpegenc_rasterize_block_422int_Cr(position_in, b, width*4);
			}
		}
		else
		{
			jpegenc_rasterize_block(position_in, b, width);
		}
	}
	else if (Gps_vec_jpeg_param_in->rotation == 1)
	{
		jpegenc_rasterize_block_rot1(position_in, b, width);
	}
	else
	{
		jpegenc_rasterize_block_rot2(position_in, b, width);
	}
#endif

	jpegenc_dct(b,a);

	/*level shifting*/
	a[0]-=1024;

	/* bit-exact to hamac */
	for(i=0;i<64;i++)
	{
        qval = a[i];
        sign = (qval<0)?-1:1;
        b[i] = sign*jpegenc_quant(sign*qval, qtable[i], 0, 0, 1);
	}

	/* HAMAC VIDEO, align DC coeff to -1023 max */
	if (b[0] <= -1024)
	{
		b[0] = -1023;
	}
	if (b[0] >= 1024)
	{
		b[0] = 1023;
	}

	/*Huffman encoding*/
	/* Encode the DC coefficient difference per section F.1.2.1 */
	temp = temp2 = b[0] - *Prec_DC;

#ifdef TRACE_ENABLE
	//printf("Pred DC Data =%x \n",*Prec_DC);
#endif

	if (temp < 0)
	{
		temp = -temp;		/* temp is abs value of input */
		/* For a negative input, want temp2 = bitwise complement of abs(input) */
		/* This code assumes we are on a two's complement machine */
		temp2--;
	}

	/* Find the number of bits needed for the magnitude of the coefficient */
	nbits = 0;
	while (temp)
	{
		nbits++;
		temp >>= 1;
	}

	/* Check for out-of-range coefficient values.
	* Since we're encoding a difference, the range limit is twice as much.
	*/
	if (nbits > 11)//MAX_COEF_BITS+1)	//MAX_COEF_BITS=10 for 8 bit
	{
#ifdef TRACE_ENABLE
		printf("Errore: dimensione > 11");
#endif
		return FALSE;
	}

	//Write the DC value
#ifdef TRACE_ENABLE
    //printf("\category:%d; val:%d; cod:%d, siz=%d",nbits,temp2,DChufftable->ehufco[nbits],DChufftable->ehufsi[nbits]);
	//printf("DC code %d size %d\n",DChufftable->ehufco[nbits], DChufftable->ehufsi[nbits]);
#endif

    jpegenc_emit_bits(DChufftable->ehufco[nbits], DChufftable->ehufsi[nbits], Gps_t1xhv_vec_jpeg_global);

	/* Emit the value (if needed)*/
    if (nbits)			/* emit_bits rejects calls with size 0 */
        jpegenc_emit_bits(temp2, nbits, Gps_t1xhv_vec_jpeg_global);


	/*update the PrecDC value*/
	*Prec_DC=b[0];

#ifdef TRACE_ENABLE
	//printf("Data %ld pixel %d \n",b[0],k);
#endif


	/* Encode the AC coefficients per section F.1.2.2 */
	r = 0;			/* r = run length of zeros */

	for (k = 1; k < 64; k++)
	{
#ifdef TRACE_ENABLE
		/*  printf("Data %ld pixel %d \n",b[zigzag_order[k]],k);     */
#endif

		if ((temp = b[jpegenc_zigzag_order[k]]) == 0)   //EL
		{
			r++;
		}
		else
		{
			/* if run length > 15, must emit special run-length-16 codes (0xF0) */
			while (r > 15)
			{
				jpegenc_emit_AC_events(15,0,0, Gps_t1xhv_vec_jpeg_global);
				r -= 16;
			}

			temp2 = temp;
			if (temp < 0)
			{
				temp = -temp;		/* temp is abs value of input */
				/* This code assumes we are on a two's complement machine */
				temp2--;
			}

			/* Find the number of bits needed for the magnitude of the coefficient */
			nbits = 1;		/* there must be at least one 1 bit */
			while ((temp >>= 1))
				nbits++;
			/* Check for out-of-range coefficient values */
			if (nbits > 10)/*//MAX_COEF_BITS)	//MAX_COEF_BITS=10 for 8 bit*/
			{
#ifdef TRACE_ENABLE
				printf("Error: DC nbits can not be grater than 10");
#endif
				return FALSE;
			};

			jpegenc_emit_AC_events((t_uint8)r,(t_uint8)nbits,(t_sint16)temp2, Gps_t1xhv_vec_jpeg_global);

			r = 0;
		}
	}


	/* If the last coef(s) were zero, emit an end-of-block code, while if nonzero a special code has to be stored */
	if(r==0)
		jpegenc_emit_AC_events(1,0,0, Gps_t1xhv_vec_jpeg_global);
	else
	{
		jpegenc_emit_AC_events(0,0,0, Gps_t1xhv_vec_jpeg_global);
	}

	return TRUE;
}

/**************************************************************************
*
*  jpegenc_emit_AC_event
*
*  write the AC coeffs of one block in the bitstream
*
*  Input:
*        AC_hufftable: Huffman table
*  Output:
*        TRUE if all ok; FALSE if an error occurs
*************************************************************************/
t_sint32 jpegenc_emit_AC_events(t_uint8	run, t_uint8 category, t_sint16	value, tps_t1xhv_vec_jpeg_global      Gps_t1xhv_vec_jpeg_global)
{
    t_uint8 i;
	jpegenc_huffman_table *AChufftable;

	AChufftable = (jpegenc_huffman_table *)Gps_t1xhv_vec_jpeg_global->AChufftable;

    i=((run)<<4)+category;

	if((i!=EOB_EVENT) & (i!=EOB1_EVENT))
    {
		//printf("AC run:%d cat:%d val:%d cod:%d siz:%d index:%d\n",run,category,value,AChufftable->ehufco[i],AChufftable->ehufsi[i],i);   */
		//printf("AC cod:%d siz:%d\n",AChufftable->ehufco[i],AChufftable->ehufsi[i]);    */
		jpegenc_emit_bits(AChufftable->ehufco[i], AChufftable->ehufsi[i], Gps_t1xhv_vec_jpeg_global);
        jpegenc_emit_bits(value,category, Gps_t1xhv_vec_jpeg_global);
    }

    if(i==EOB_EVENT)
    {
        jpegenc_emit_bits(AChufftable->ehufco[0],AChufftable->ehufsi[0], Gps_t1xhv_vec_jpeg_global);
    }

    return TRUE;
}

void jpegenc_write_n_bytes(t_uint32 nbytes, t_uint8 *p_hdr, tps_t1xhv_vec_jpeg_global      Gps_t1xhv_vec_jpeg_global)
{
	while(nbytes!=0)
	{
		jpegenc_emit_byte(*p_hdr,Gps_t1xhv_vec_jpeg_global);
		p_hdr++;nbytes--;
	}
}

#ifdef TRACE_ENABLE

#define CHECK_SOFT_ERR(data,shift) (data & ((1 << shift) - 1))

t_uint16 jpegenc_check_error(tps_t1xhv_vec_jpeg_session Gps_vec_jpeg_session)
{
    t_uint16 error_type = 0;
    t_uint16 i=0;

    if (CHECK_SOFT_ERR(Gps_vec_jpeg_session->Gps_vec_jpeg_frame_buf_in->addr_source_buffer,8))
        error_type= VEC_ERT_ADDR_SOURCE_BUFFER;

    /* -0x0081 =addr_fwd_ref_buffer is not a multiple of 256 */
    if (CHECK_SOFT_ERR(Gps_vec_jpeg_session->Gps_vec_jpeg_frame_buf_in->addr_fwd_ref_buffer,8))
        error_type= VEC_ERT_ADDR_FWD_REF_BUFFER;

    /* -0x0082 =addr_grab_ref_buffer is not a multiple of 256 */
    if (CHECK_SOFT_ERR(Gps_vec_jpeg_session->Gps_vec_jpeg_frame_buf_in->addr_grab_ref_buffer,8))
        error_type= VEC_ERT_ADDR_GRAB_REF_BUFFER;

    /* -0x0087 =addr_header_buffer is not a multiple of 16 */
    if (CHECK_SOFT_ERR(Gps_vec_jpeg_session->Gps_jpeg_bitstream_buf_header->addr_header_buffer,4))
        error_type= VEC_ERT_ADDR_HEADER_BUFFER;

    /* -0x0088 =addr_bistream_start is not a multiple of 16 */
    if (CHECK_SOFT_ERR(Gps_vec_jpeg_session->Gps_jpeg_bitstream_buf_pos->addr_bitstream_start,4))
        error_type= VEC_ERT_ADDR_BITSTREAM_START;

    /* -0x00a0 =frame_width is not a multiple of 16 */
    if (CHECK_SOFT_ERR(Gps_vec_jpeg_session->Gps_vec_jpeg_param_in->frame_width,4))
        error_type= VEC_ERT_FRAME_WIDTH;

    /* -0x00a1 =frame_height is not a multiple of 16 */
    if (CHECK_SOFT_ERR(Gps_vec_jpeg_session->Gps_vec_jpeg_param_in->frame_height,4))
        error_type= VEC_ERT_FRAME_HEIGHT;

    /* -0x00a2 =window_width is not a multiple of 8 */
    if (CHECK_SOFT_ERR(Gps_vec_jpeg_session->Gps_vec_jpeg_param_in->window_width,3))
        error_type= VEC_ERT_WINDOW_WIDTH;

    /* -0x00a3 =window_height is not a multiple of 8 */
    if (CHECK_SOFT_ERR(Gps_vec_jpeg_session->Gps_vec_jpeg_param_in->window_height,3))
        error_type= VEC_ERT_WINDOW_HEIGHT;

    for (i==0 ; i < 64 ; i++)
    {
        /* -0x00c2 =quant_luma is equal to 0 */
        if (Gps_vec_jpeg_session->Gps_vec_jpeg_param_in->quant_luma[i] == 0)
            error_type= VEC_ERT_QUANT_TABLE;

        /* -0x00c2 =quant_chroma is equal to 0 */
        if (Gps_vec_jpeg_session->Gps_vec_jpeg_param_in->quant_chroma[i] == 0)
            error_type= VEC_ERT_QUANT_TABLE;
    }

    /* rotation value should be 0,1,2 */
    if ((Gps_vec_jpeg_session->Gps_vec_jpeg_param_in->rotation != 0) && (Gps_vec_jpeg_session->Gps_vec_jpeg_param_in->rotation != 1) && (Gps_vec_jpeg_session->Gps_vec_jpeg_param_in->rotation != 2))
		error_type = VEC_ERT_BAD_PARAMETER;

    return error_type;
}
#endif


