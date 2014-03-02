/*
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*/

#ifdef __NMF

#include "stdio.h"
#include <mpeg4dec/arm_nmf/algo.nmf>
#include "types.h"	
#include "t1xhv_common.idt"	
#include "t1xhv_vdc_mpeg4.idt"
#include "videocodecDecApi.h"
#include "videocodecMotionCompensation.h"

#ifdef _DEBUG_SOFT
#include <los/api/los_api.h>
#endif

/*------------------------------------------------------------------------
* Global Variables
*----------------------------------------------------------------------*/


/*------------------------------------------------------------------------
* Private functions prototype
*----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
Methods of component interface
*----------------------------------------------------------------------*/

/*****************************************************************************/
/**
* \brief  init
*
* Init jpeg Decoder Algo component. Implicitly called while instanciating
*   this component.
*   Unmask wanted internal interrupts.
*/
/*****************************************************************************/


t_nmf_error mpeg4dec_arm_nmf_algo::construct()
{
	return NMF_OK;
}

#ifdef _DEBUG_SOFT
static unsigned long long _endtime, _starttime, totaltime=0L;
#endif	
/*****************************************************************************/
/**
* \brief  controlAlgo
*
* VP6 Decoder : Control Algo component.
*
* \param   sCommand
* \param   param (n/a)
*/
/*****************************************************************************/
void mpeg4dec_arm_nmf_algo::controlAlgo(t_t1xhv_command command, t_uint16 channelNumber, t_uint16 param)
{


#ifdef _DEBUG_SOFT
	printf("Hi mpeg4dec_arm_nmf_algo::controlAlgo with command = %d\n",command);
#endif //ifdef _DEBUG_SOFT
	//printf("\n\n ******** sizeof(InstreamBuffer_t)=%d *******\n\n", sizeof(InstreamBuffer_t));
	//printf("\n\n ******** sizeof(ParseEvent_t)=%d *******\n\n", sizeof(ParseEvent_t));
	//printf("\n\n ******** sizeof(jmp_buf)=%d *******\n\n", sizeof(jmp_buf));




	switch (command)
	{
	case CMD_RESET:
	case CMD_ABORT:
		/* (Not yet implemented) */
		break;

	case CMD_START:
		{

			t_uint32 res, ConsumedBytes;
			mp4d_SetMotionParams(LayerData_p, &LayerData_p->MC_Params);
#ifdef _DEBUG_SOFT
			_starttime= LOS_getSystemTime();
#endif
			res = mp4d_VideoDecDecodePictureAlgo(LayerData_p, &ConsumedBytes);

#ifdef _DEBUG_SOFT
			_endtime = LOS_getSystemTime() - _starttime;
			totaltime = totaltime+_endtime;
			printf("Total time in microsec = %lld\n",totaltime);
#endif			
			mp4d_ResetBeforePicture(LayerData_p,param);
			oEndAlgo.endAlgo(res == 0 ? STATUS_JOB_COMPLETE:STATUS_JOB_ABORTED,(t_t1xhv_decoder_info)0,0,ConsumedBytes);
		}
		break;
	default:

		break;
	}


} /* end of controlAlgo() function */


void mpeg4dec_arm_nmf_algo::configureAlgo(t_uint32 addr_in_frame_buffer,
										  t_uint32 addr_out_frame_buffer,
										  t_uint32 addr_internal_buffer,
										  t_uint32 addr_in_bitstream_buffer,
										  t_uint32 addr_out_bitstream_buffer,
										  t_uint32 addr_in_parameters,
										  t_uint32 addr_out_parameters,
										  t_uint32 addr_in_frame_parameters,
										  t_uint32 addr_out_frame_parameters)
{
	t_uint32 temp2;
	t_uint32 tmp_current_time=0;

	ts_t1xhv_vdc_mpeg4_param_in    *Gp_vdc_mpeg4_param_in;// = LayerData_p->Gp_vdc_mpeg4_param_in;
	InstreamBuffer_t* Instream_p;

	LayerData_p->Gp_vdc_mpeg4_frame_buf_in  = (ts_t1xhv_vdc_frame_buf_in*)addr_in_frame_buffer;
	LayerData_p->Gp_vdc_mpeg4_frame_buf_out = (ts_t1xhv_vdc_frame_buf_out*)addr_out_frame_buffer;
	LayerData_p->Gp_vdc_mpeg4_internal_buf  = (ts_t1xhv_vdc_internal_buf *)addr_internal_buffer;
	LayerData_p->Gp_vdc_mpeg4_bitstream_buf_pos  = (ts_t1xhv_bitstream_buf_pos *)addr_in_bitstream_buffer;
	LayerData_p->Gp_vdc_mpeg4_param_in			= (ts_t1xhv_vdc_mpeg4_param_in*)addr_in_parameters;
	LayerData_p->Gp_vdc_mpeg4_param_out			= (ts_t1xhv_vdc_mpeg4_param_out*)addr_out_parameters;
	LayerData_p->Gp_vdc_mpeg4_param_inout_in		= (ts_t1xhv_vdc_mpeg4_param_inout*)addr_in_frame_parameters;
	LayerData_p->Gp_vdc_mpeg4_param_inout_out		= (ts_t1xhv_vdc_mpeg4_param_inout*)addr_out_frame_parameters;


	mp4d_VideoDecInitDecoder(LayerData_p); 

	StartupInstream(LayerData_p); 

	Gp_vdc_mpeg4_param_in = LayerData_p->Gp_vdc_mpeg4_param_in;
	Instream_p = LayerData_p->Instream_p;

	Instream_p->CurrentBuffer_p = (t_uint8*)(LayerData_p->Gp_vdc_mpeg4_bitstream_buf_pos->addr_bitstream_start)+ (LayerData_p->Gp_vdc_mpeg4_bitstream_buf_pos->bitstream_offset / 32)*4;
	Instream_p->Buffer_p = Instream_p->CurrentBuffer_p;
	t_uint8 temp = *((t_uint8*)Instream_p->CurrentBuffer_p);
	Instream_p->CurrentBuffer_p +=1;
	temp2 = temp;

	temp = *((t_uint8*)Instream_p->CurrentBuffer_p);
	temp2 = (temp2<<8) | temp;
	Instream_p->CurrentBuffer_p +=1;

	temp = *((t_uint8*)Instream_p->CurrentBuffer_p);
	temp2 = (temp2<<8) | temp;
	Instream_p->CurrentBuffer_p +=1;

	temp = *((t_uint8*)Instream_p->CurrentBuffer_p);
	temp2 = (temp2<<8) | temp;
	Instream_p->CurrentBuffer_p +=1;

	Instream_p->Buffer32_Pos = 32- (LayerData_p->Gp_vdc_mpeg4_bitstream_buf_pos->bitstream_offset % 32);
	Instream_p->Buffer32 = temp2 << (32-Instream_p->Buffer32_Pos);

	Instream_p->BufferEnd_p = (t_uint8*)((ts_t1xhv_bitstream_buf_link*)LayerData_p->Gp_vdc_mpeg4_bitstream_buf_pos->addr_bitstream_buf_struct)->addr_buffer_end;
	Instream_p->CurrentStartcode = (ParseEvent_t)(PSC);

	LayerData_p->CurrRecImage.Y_p = (uint8 *)LayerData_p->Gp_vdc_mpeg4_frame_buf_out->addr_dest_buffer;
	LayerData_p->CurrRecImage.Cb_p        = LayerData_p->CurrRecImage.Y_p + (Gp_vdc_mpeg4_param_in->frame_width * Gp_vdc_mpeg4_param_in->frame_height);
	LayerData_p->CurrRecImage.Y_LineWidth = Gp_vdc_mpeg4_param_in->frame_width;   // Distance to next row
#ifdef _CB_CR_INTERLEAVE_	
	LayerData_p->CurrRecImage.Cr_p        =  LayerData_p->CurrRecImage.Cb_p ;
	LayerData_p->CurrRecImage.C_LineWidth = Gp_vdc_mpeg4_param_in->frame_width ;
	LayerData_p->CurrRecImage.C_DispWidth = Gp_vdc_mpeg4_param_in->frame_width / 2;
#else
	LayerData_p->CurrRecImage.Cr_p        =  LayerData_p->CurrRecImage.Cb_p + ((Gp_vdc_mpeg4_param_in->frame_width*Gp_vdc_mpeg4_param_in->frame_height)/4);
	LayerData_p->CurrRecImage.C_LineWidth = Gp_vdc_mpeg4_param_in->frame_width / 2;
	LayerData_p->CurrRecImage.C_DispWidth = Gp_vdc_mpeg4_param_in->frame_width / 2;
#endif 
	LayerData_p->CurrRecImage.Y_DispWidth = Gp_vdc_mpeg4_param_in->frame_width;   // The number pixels on a line to display

	LayerData_p->CurrRecImage.Y_Height    = Gp_vdc_mpeg4_param_in->frame_height;
	LayerData_p->CurrRecImage.C_Height    = Gp_vdc_mpeg4_param_in->frame_height / 2;

	LayerData_p->PrevRecImage.Y_p = (uint8 *)LayerData_p->Gp_vdc_mpeg4_frame_buf_in->addr_fwd_ref_buffer;
	LayerData_p->PrevRecImage.Cb_p        = LayerData_p->PrevRecImage.Y_p + (Gp_vdc_mpeg4_param_in->frame_width * Gp_vdc_mpeg4_param_in->frame_height);
	LayerData_p->PrevRecImage.Y_LineWidth = Gp_vdc_mpeg4_param_in->frame_width;   // Distance to next row
#ifdef _CB_CR_INTERLEAVE_	
	LayerData_p->PrevRecImage.Cr_p        = LayerData_p->PrevRecImage.Cb_p ;
	LayerData_p->PrevRecImage.C_LineWidth = Gp_vdc_mpeg4_param_in->frame_width;
	LayerData_p->PrevRecImage.C_DispWidth = Gp_vdc_mpeg4_param_in->frame_width / 2;
#else
	LayerData_p->PrevRecImage.Cr_p        = LayerData_p->PrevRecImage.Cb_p + ((Gp_vdc_mpeg4_param_in->frame_width*Gp_vdc_mpeg4_param_in->frame_height)/4);
	LayerData_p->PrevRecImage.C_LineWidth = Gp_vdc_mpeg4_param_in->frame_width / 2;
	LayerData_p->PrevRecImage.C_DispWidth = Gp_vdc_mpeg4_param_in->frame_width / 2;
#endif
	LayerData_p->PrevRecImage.Y_DispWidth = Gp_vdc_mpeg4_param_in->frame_width;   // The number pixels on a line to display

	LayerData_p->PrevRecImage.Y_Height    = Gp_vdc_mpeg4_param_in->frame_height;
	LayerData_p->PrevRecImage.C_Height    = Gp_vdc_mpeg4_param_in->frame_height / 2;

	LayerData_p->FutureRecImage.Y_p = (uint8 *)LayerData_p->Gp_vdc_mpeg4_frame_buf_in->addr_bwd_ref_buffer;
	LayerData_p->FutureRecImage.Cb_p        = LayerData_p->FutureRecImage.Y_p + (Gp_vdc_mpeg4_param_in->frame_width * Gp_vdc_mpeg4_param_in->frame_height);
	LayerData_p->FutureRecImage.Y_LineWidth = Gp_vdc_mpeg4_param_in->frame_width;   // Distance to next row
#ifdef _CB_CR_INTERLEAVE_	
	LayerData_p->FutureRecImage.Cr_p        = LayerData_p->FutureRecImage.Cb_p ;
	LayerData_p->FutureRecImage.C_LineWidth = Gp_vdc_mpeg4_param_in->frame_width;
	LayerData_p->FutureRecImage.C_DispWidth = Gp_vdc_mpeg4_param_in->frame_width / 2;
#else
	LayerData_p->FutureRecImage.Cr_p        = LayerData_p->FutureRecImage.Cb_p + ((Gp_vdc_mpeg4_param_in->frame_width*Gp_vdc_mpeg4_param_in->frame_height)/4);
	LayerData_p->FutureRecImage.C_LineWidth = Gp_vdc_mpeg4_param_in->frame_width / 2;
	LayerData_p->FutureRecImage.C_DispWidth = Gp_vdc_mpeg4_param_in->frame_width / 2;
#endif
	LayerData_p->FutureRecImage.Y_DispWidth = Gp_vdc_mpeg4_param_in->frame_width;   // The number pixels on a line to display

	LayerData_p->FutureRecImage.Y_Height    = Gp_vdc_mpeg4_param_in->frame_height;
	LayerData_p->FutureRecImage.C_Height    = Gp_vdc_mpeg4_param_in->frame_height / 2;
	LayerData_p->NrMB= (Gp_vdc_mpeg4_param_in->frame_width /16) * (Gp_vdc_mpeg4_param_in->frame_height /16);

	Instream_p->Standard = (Standard_t)(Gp_vdc_mpeg4_param_in->flag_short_header == 1 ? 1:2);
#ifdef _ALGO_DEBUG
	Instream_p->debugFrameCounter = LayerData_p->Gp_vdc_mpeg4_param_inout_in->reserved_1;

	//printf("LayerData_p->Gp_vdc_mpeg4_param_inout_in->reserved_1=%x\n",LayerData_p->Gp_vdc_mpeg4_param_inout_in->reserved_1);

	printf("Algo: debugFrameCounter=%x\n",Instream_p->debugFrameCounter);
#endif
	if(Gp_vdc_mpeg4_param_in->picture_coding_type != 2)
	{
		LayerData_p->Gp_vdc_mpeg4_param_inout_out->forward_time  = LayerData_p->Gp_vdc_mpeg4_param_inout_in->backward_time;
		LayerData_p->Gp_vdc_mpeg4_param_inout_out->backward_time = (t_sint32)((t_uint32)Gp_vdc_mpeg4_param_in->vop_time_increment +
			(t_uint32)Gp_vdc_mpeg4_param_in->modulo_time_base*
			(t_uint32)Gp_vdc_mpeg4_param_in->vop_time_increment_resolution);
	}
	else
	{
		tmp_current_time = (t_uint32)((t_uint32)Gp_vdc_mpeg4_param_in->vop_time_increment 
			+(t_uint32) Gp_vdc_mpeg4_param_in->modulo_time_base
			*(t_uint32)Gp_vdc_mpeg4_param_in->vop_time_increment_resolution);

		if (tmp_current_time > LayerData_p->Gp_vdc_mpeg4_param_inout_in->forward_time)
		{
			/* this is the nominal case */
			LayerData_p->TRB = tmp_current_time - LayerData_p->Gp_vdc_mpeg4_param_inout_in->forward_time;
		}
		else
		{
			/* this is a strange case... */
			LayerData_p->TRB = tmp_current_time +
				(t_uint32)Gp_vdc_mpeg4_param_in->vop_time_increment_resolution 
				-  LayerData_p->Gp_vdc_mpeg4_param_inout_in->forward_time;
		}

		if (LayerData_p->Gp_vdc_mpeg4_param_inout_in->backward_time > LayerData_p->Gp_vdc_mpeg4_param_inout_in->forward_time)
		{
			/* normal case */
			LayerData_p->TRD =  LayerData_p->Gp_vdc_mpeg4_param_inout_in->backward_time - LayerData_p->Gp_vdc_mpeg4_param_inout_in->forward_time;
		}
		else
		{
			/* strange case */
			LayerData_p->TRD = (LayerData_p->Gp_vdc_mpeg4_param_inout_in->backward_time + Gp_vdc_mpeg4_param_in->vop_time_increment_resolution) - LayerData_p->Gp_vdc_mpeg4_param_inout_in->forward_time;
		}
	}

	//printf("Config start 3 AIC_Data_p=%x, AIC_Data_p->Ap=%x\n", LayerData_p->AIC_Data_p, LayerData_p->AIC_Data_p->Ap);

	LayerData_p->NrMB_Horizontal = Gp_vdc_mpeg4_param_in->frame_width /16;
	if(Instream_p->Standard == MPEG4 || Gp_vdc_mpeg4_param_in->flag_sorenson || Gp_vdc_mpeg4_param_in->frame_height <= 400)
	{
		LayerData_p->NrMB_InGOB    =Gp_vdc_mpeg4_param_in->frame_width /16;
		LayerData_p->NumberOf_GOBS = Gp_vdc_mpeg4_param_in->frame_height /16;
	}
	else if (Gp_vdc_mpeg4_param_in->frame_height <= 800)
	{
		LayerData_p->NrMB_InGOB    = (LayerData_p->NrMB)/(LayerData_p->NrMB_Horizontal*2);
		LayerData_p->NumberOf_GOBS = LayerData_p->NrMB/LayerData_p->NrMB_InGOB;
	}
	else
	{
		LayerData_p->NrMB_InGOB    = (LayerData_p->NrMB)/(LayerData_p->NrMB_Horizontal*4);
		LayerData_p->NumberOf_GOBS = LayerData_p->NrMB/LayerData_p->NrMB_InGOB;
	}
	LayerData_p->MB_History_p = (MB_Data_t *)LayerData_p->Gp_vdc_mpeg4_internal_buf->addr_mv_type_buffer;

	//LayerData_p->PictClockFreq.Divisor = Gp_vdc_mpeg4_param_in->vop_time_increment_resolution;
	//LayerData_p->PictClockFreq.ConversionCode = 1000;
	//printf("Configure end AIC_Data_p=%x, AIC_Data_p->Ap=%x\n", LayerData_p->AIC_Data_p, LayerData_p->AIC_Data_p->Ap);

} /* end of configureAlgo() function. */

void mpeg4dec_arm_nmf_algo::updateAlgo(t_t1xhv_command command)
{
#ifdef _DEBUG_SOFT
	printf("Hi mpeg4dec_arm_nmf_algo::updateAlgo\n");
#endif //ifdef _DEBUG_SOFT


}


/*------------------------------------------------------------------------
* Internal Functions
*----------------------------------------------------------------------*/

void mpeg4dec_arm_nmf_algo::setDebug(t_t1xhv_debug_mode mode, t_uint32 param1, t_uint32 param2)
{
#ifdef _DEBUG_SOFT
	printf("Hi mpeg4dec_arm_nmf_algo::setDebug\n");
#endif //ifdef _DEBUG_SOFT

}


#endif




