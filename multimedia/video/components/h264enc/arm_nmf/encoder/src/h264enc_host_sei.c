/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "simple_types.h"
#include "host_global.h"
#include "host_sei.h"
#include "host_vlc.h"
#include "host_annexb.h"
#include "error.h"*/

#include "types.h"
#include "h264enc_host_sei.h"

#include <t1xhv_common.idt>
#include <t1xhv_vec_h264.idt>

/*typedef struct hamac_param
{
    ts_t1xhv_vec_frame_buf_in*	 	 addr_in_frame_buffer;                            
    ts_t1xhv_vec_frame_buf_out*	 	 addr_out_frame_buffer;                            
    ts_t1xhv_vec_internal_buf*	 	 addr_internal_buffer;                            
    ts_t1xhv_bitstream_buf_pos*	 	 addr_in_bitstream_buffer;                            
    ts_t1xhv_bitstream_buf_pos*	 	 addr_out_bitstream_buffer;                            
    ts_t1xhv_vec_h264_param_in*	 	 addr_in_parameters;
    ts_t1xhv_vec_h264_param_out*	 addr_out_parameters;
    ts_t1xhv_vec_h264_param_inout* 	 addr_in_frame_parameters;
    ts_t1xhv_vec_h264_param_inout*	 addr_out_frame_parameters;

    ts_t1xhv_bitstream_buf*              addr_bitstream_buf_struct;
}*/

t_uint8  rbsp[NONVCL_BUFFER_SIZE]; /* NZ: statically alloc'ed buffer for non-VCL NAL unit EXCEPT Filled Data NAL Units */
t_uint8  picture_timing_rbsp[PT_SEI_RBSP_BUFFER_SIZE]; /* NZ: statically alloc'ed buffer for SEI NALU rbsp */
t_uint8  buffering_period_rbsp[BP_SEI_RBSP_BUFFER_SIZE]; /* NZ: statically alloc'ed buffer for SEI NALU rbsp */
t_uint8  stereo_video_rbsp[SV_SEI_RBSP_BUFFER_SIZE]; 
t_uint8  frame_packing_rbsp[FP_SEI_RBSP_BUFFER_SIZE];

ts_t1xhv_vec_h264_param_in* Gsp_img;
ts_t1xhv_vec_h264_param_inout* Gsp_inout;
/*
* local function prototype 
*/
void GenerateSEI_NALU (tps_NALU nalu);
t_sint32 GenerateSEI_rbsp (t_uint8* rbsp);
t_uint16 EstimateBP(void);
t_uint16 EstimatePT(void);



/* Buffering Period functions prototypes */
void InitBufferingPeriod(void);
void UpdateBufferingPeriod(t_BRC_SEI* SEIio);
void FinalizeBufferingPeriod(void);

/* Picture Timing functions prototypes */
void InitPictureTiming(void);
void UpdatePictureTiming(t_BRC_SEI* SEIio);
void FinalizePictureTiming(void);
t_uint16 getNumClockTS(t_uint16 pic_struct);




/*
* global data
*/

t_uint16 sei_message_payloadSize;  /* size of SEI Payload */
static t_BRC_SEI BRCtoSEI;                /* from BRC to SEI struct and pointer to */
t_BRC_SEI* SEIio =  &BRCtoSEI;

static t_buffering_period_sei buffering_period_sei; /* BUFFERING PERIOD SEI MESSAGE */
static t_picture_timing_sei picture_timing_sei;     /* PICTURE TIMING SEI MESSAGE */
//static t_stereo_video_sei stereo_video_sei;         /* STEREO VIDEO INFORMATION SEI MESSAGE */
//static t_frame_packing_sei frame_packing_sei;       /* FRAME PACKING ARRANGEMENT SEI MESSAGE */


t_uint32 SEIsize = 0;

/* FIXME */
seq_parameter_set_rbsp_t* Gsp_sps;

/*!
 ************************************************************************
 *  \brief
 *     initialize message type dependent data
 *  \note 
 *    Progressively supported SEI messages shall be initialized by calling 
 *    the proper function here
 *    
 ************************************************************************
 */
void InitSEIMessages() {
  sei_message_payloadSize = 0;
  InitBufferingPeriod();
  InitPictureTiming();
  //if (Gsp_input->3DFormatType != R)
}


/*!
 ************************************************************************
 *  \brief
 *     write one sei payload to the sei message
 *  \param destination
 *    a pointer that point to the sei NALU payload. Note that the bitstream
 *    should have be byte aligned already. 
 *  \param payload_type
 *    the type of the sei payload
 *  \par Output
 *    the content of the sei message (destination) is updated.
 ************************************************************************
 */
void write_sei_message(t_uint8* destination, SEI_type payload_type) {
  t_uint16 offset, size;
  SEI_type type;
  t_uint8* payload = (t_uint8*)NULL;
  t_uint16 payload_size = 0;
  t_uint16 i;

  switch (payload_type) {
    case SEI_BUFFERING_PERIOD:
      payload = buffering_period_sei.data.streamBuffer;
      payload_size = buffering_period_sei.payloadSize;
      break;
    case SEI_PICTURE_TIMING:
      payload = picture_timing_sei.data.streamBuffer;
      payload_size = picture_timing_sei.payloadSize;
      break;
    default:
      //error("\nSEI message type not allowed", -1);
      printf("\nSEI message type not allowed", -1);
    break;
  }

  type = payload_type;
  size = payload_size;
  offset = sei_message_payloadSize;

  while ( type > 255 ) {
    destination[offset++] = 0xFF;
    type = (SEI_type)(type - 255);
  }
  destination[offset++] = type;

  while ( size > 255 ) {
    destination[offset++] = 0xFF;
    size = size - 255;
  }
  destination[offset++] = (t_uint8) size;
  /* copy from sei message dependant buffer to NALU stream buffer */
  for (i=0; i<payload_size; i++)
    *(destination + offset + i) = *(payload+i);
    offset += payload_size;
  sei_message_payloadSize = offset;
}

#if (SEI_CPB_REMOVAL_DELAY_BITS < SEI_CPB_REMOVAL_DELAY_BITS)
#define LIMIT   1<<(SEI_CPB_REMOVAL_DELAY_BITS-1)
#else
#define LIMIT   1<<(SEI_DPB_REMOVAL_DELAY_BITS-1)
#endif


/*!
 ************************************************************************
 *  \brief
 *     - Decide whether to force a Buffering Period SEI message in current AU
 *  \par
 *     - SEIio struct pointer
 ***********************************************************************
 */
void InitSEIio(t_BRC_SEI* SEIio)
{
  /* NZ: 
   * 1<<(SEI_CPB_REMOVAL_DELAY_BITS-1) could be 1<<(SEI_CPB_REMOVAL_DELAY_BITS) 
   * in fact, I always DO NOT USE a bit (the fist one).
   * That's because I can detect the wrap-around only when succeded, and 
   * in  that case it will be too late to start the new buffering period, 
   * because the removal time of a AU with a Buffering Period within is coded 
   * as a delta from previous one. Avoid to use one bit I avoid the wrap around 
   * for this Buffering Period AU.
   */
   SEIio->forceBP = (((SEIio->currAUts - SEIio->lastBPAUts) > LIMIT)? 1 : 0 ) || Gsp_img->idr_flag ;
}



/*!
 ************************************************************************
 *  \brief
 *     - Convert Buffering Period SEI message data to bitstream syntax data
 *     - Close the SEI message rspb
 *     - Convert the SEI rspb to NALU
 *     - Write the NALU in the bistream
 *  \par
 *     - SEIio struct pointer
 ***********************************************************************
 */
void WriteSEI (t_BRC_SEI* SEIio) {
  //t_uint16 len;
  ts_NALU   nalu_s;
  tps_NALU nalu = &nalu_s;

  /* NZ: rbsp buffer statically alloc'ed in host_nalucommon.h */
  nalu->buf = &rbsp[0];

  GenerateSEI_NALU(nalu);        /* generate SEI NAL */

  //len = WriteNALU (NON_VCL_NALU, nalu);
 
  sei_message_payloadSize = 0; /* reset payload size (ready for next SEI NALU) */
  nalu = (tps_NALU)NULL;
}

/*!
 ************************************************************************
 *  \brief
 *     Update the picture timing field used to store the final arrival time 
 *     of current AU.
 *     This function MUST be called after ALL NALU of the AU are write/buffered
 *     The size of the whole AU (every NALU) must be stored in SEIio->NALAUsize.
 *     The size of the VCL NALU inside current AU must be stored 
 *     in SEIio->VCLAUsize.
 *  \par
 *     - SEIio struct pointer
  ************************************************************************
 */
void UpdateFinalArrivalTime(t_BRC_SEI* SEIio) {
  t_uint16 nalcnt, vclcnt, i;
  t_uint32 BitRate;
  t_uint16 bit_rate_scale;
  t_uint16 bit_rate_value;
  double   myLength;
  
  nalcnt = Gsp_sps->vui_seq_parameters.nal_hrd_parameters.cpb_cnt_minus1+1;
  vclcnt = Gsp_sps->vui_seq_parameters.vcl_hrd_parameters.cpb_cnt_minus1+1;

  for (i=0; i < nalcnt; i++) {
    bit_rate_scale = Gsp_sps->vui_seq_parameters.nal_hrd_parameters.bit_rate_scale;
	  bit_rate_value = Gsp_sps->vui_seq_parameters.nal_hrd_parameters.bit_rate_value_minus1[i] + 1;
	  BitRate = bit_rate_value * (1 << (bit_rate_scale + 6));

    myLength = ((double)SEIio->NALAUsize / (double)BitRate);
    picture_timing_sei.NALfinal_arrival_time[i] += myLength;
  }

  for (i=0; i < vclcnt; i++) {
	  bit_rate_scale = Gsp_sps->vui_seq_parameters.vcl_hrd_parameters.bit_rate_scale;
	  bit_rate_value = Gsp_sps->vui_seq_parameters.vcl_hrd_parameters.bit_rate_value_minus1[0] + 1;
	  BitRate = bit_rate_value * (1 << (bit_rate_scale + 6));
    
    myLength = ((double)SEIio->VCLAUsize / (double)BitRate);
	  picture_timing_sei.VCLfinal_arrival_time[i] += myLength;	
  }
  Gsp_img->NALfinal_arrival_time = (t_uint32)(picture_timing_sei.NALfinal_arrival_time[0] * 4096);
}



/*!
 ************************************************************************
 *  \brief
 *     Generate an appropriate NALU from SEI message rspb data 
 *  \return 
 *     pointer to SEI NALU
  ************************************************************************
 */
void GenerateSEI_NALU (tps_NALU p_nalu) {
  t_sint32 RBSPlen = 0;
  t_uint16 len;

  ASSERT (nalu != NULL);

  p_nalu->forbidden_bit = 0;
  p_nalu->nal_reference_idc = NALU_PRIORITY_DISPOSABLE;
  p_nalu->nal_unit_type = NALU_TYPE_SEI;
  p_nalu->startcodeprefix_len = 4;
  p_nalu->buf[0] =
    p_nalu->forbidden_bit << 7      |
    p_nalu->nal_reference_idc << 5  |
    p_nalu->nal_unit_type;

  RBSPlen = GenerateSEI_rbsp (&(p_nalu->buf[1]));
  len = 1 + RBSPtoEBSP (& (p_nalu->buf[1]), 0, RBSPlen, 0);
  p_nalu->len = len;
}



/*!
 ************************************************************************
 *  \brief
 *     Generate SEI message rspb data from SEI structures
 *  \return 
 *     pointer to SEI NALU payload buffer
  ************************************************************************
 */
t_sint32 GenerateSEI_rbsp (t_uint8 *rbsp) {

  if (SEIio->InitBuffer)
    SEIio->lastBPAUts  = SEIio->currAUts;
  
  if (SEIio->forceBP)
    UpdateBufferingPeriod(SEIio);

  if (!SEIio->InitBuffer)
  UpdatePictureTiming(SEIio);

  if (SEIio->forceBP) {
        FinalizeBufferingPeriod();
        write_sei_message(rbsp, SEI_BUFFERING_PERIOD);
  }
  /* always send picture timing SEI message */
  FinalizePictureTiming();
  write_sei_message(rbsp, SEI_PICTURE_TIMING);

  rbsp[sei_message_payloadSize] = 0x80;
  sei_message_payloadSize++;           /* Add last 0x80 byte to SEI NAL payload */
  return sei_message_payloadSize;
}




/********************************/
/* BUFFERING PERIOD SEI MESSAGE */
/********************************/

/*!
 ************************************************************************
 *  \brief
 *     Init the global variable for Buffering Period SEI message
  ************************************************************************
 */
void InitBufferingPeriod()
{
    t_uint16 i;

    buffering_period_sei.seq_parameter_set_id = Gsp_sps->seq_parameter_set_id;

    for (i=0; i < Gsp_sps->vui_seq_parameters.nal_hrd_parameters.cpb_cnt_minus1+1; i++)
    {
        buffering_period_sei.NALinitial_cpb_removal_delay[i] = 0xaaaaaaaa & ((1<<SEI_INITIAL_CPB_REMOVAL_DELAY_BITS)-1);
        buffering_period_sei.NALinitial_cpb_removal_delay_offset[i] = 0xaaaaaaaa & ((1<<SEI_INITIAL_CPB_REMOVAL_DELAY_BITS)-1);
    }

    for (i=0; i < Gsp_sps->vui_seq_parameters.vcl_hrd_parameters.cpb_cnt_minus1+1; i++)
    {
        buffering_period_sei.VCLinitial_cpb_removal_delay[i] = 0xaaaaaaaa & ((1<<SEI_INITIAL_CPB_REMOVAL_DELAY_BITS)-1);
        buffering_period_sei.VCLinitial_cpb_removal_delay_offset[i] = 0xaaaaaaaa & ((1<<SEI_INITIAL_CPB_REMOVAL_DELAY_BITS)-1);
    }
    
    buffering_period_sei.NAL_lastBPAUremoval_time = 0;
    buffering_period_sei.VCL_lastBPAUremoval_time = 0;

    buffering_period_sei.data.bits_to_go  = 8;
    buffering_period_sei.data.byte_pos    = 0;
    buffering_period_sei.data.byte_buf    = 0;
    buffering_period_sei.data.streamBuffer = &buffering_period_rbsp[0];
    for (i=0; i<BP_SEI_RBSP_BUFFER_SIZE; i++)
      buffering_period_sei.data.streamBuffer[i] = 0;
}

/*!
 ************************************************************************
 *  \brief
 *     Update global variable for Buffering Period SEI message
  ************************************************************************
 */
void UpdateBufferingPeriod(t_BRC_SEI* SEIio)
{
  t_uint16 i;
  t_uint16 nalcnt, vclcnt;
  double tc;
  double tmp1, tmp2;
  double delta;

  nalcnt = Gsp_sps->vui_seq_parameters.nal_hrd_parameters.cpb_cnt_minus1+1;
  vclcnt = Gsp_sps->vui_seq_parameters.vcl_hrd_parameters.cpb_cnt_minus1+1;
  tc = (double)  Gsp_sps->vui_seq_parameters.num_units_in_tick / (double) Gsp_sps->vui_seq_parameters.time_scale;

  buffering_period_sei.seq_parameter_set_id = Gsp_sps->seq_parameter_set_id;

  for (i=0; i < nalcnt; i++) {
    if (SEIio->InitBuffer) {
#ifdef __ndk5500_a0__
        buffering_period_sei.NALinitial_cpb_removal_delay[i] = (t_uint32) ((double)SEIio->currAUts * (double)90000.0 * 1024 * (double) Gsp_img->framerate_den / (double) /*Gsp_h264Global*/Gsp_img->framerate_num);
#else
        buffering_period_sei.NALinitial_cpb_removal_delay[i] = (t_uint32) ((double)SEIio->currAUts * (double)90000.0 * 1024 / (double) /*Gsp_h264Global*/Gsp_img->framerate);
#endif
		  /* buffering_period_sei.NALinitial_cpb_removal_delay_offset[i] = buffering_period_sei.NALinitial_cpb_removal_delay[i]; */
      /* NZ: NALinitial_cpb_removal_delay_offset is not used when cbr_flag = 0 (see eq. C-3 and sub);
        to avoid the emulation prevention byte a 10101... patern is inserted */
        buffering_period_sei.NALinitial_cpb_removal_delay_offset[i] = 0xaaaaaaaa & ((1<<SEI_INITIAL_CPB_REMOVAL_DELAY_BITS)-1);
      buffering_period_sei.NAL_lastBPAUremoval_time = (double)SEIio->currAUts * tc;
	}
    else {
      /* NZ: this chuck of code, which appear a little redundant, is so written to force
      the write of partial result to memory, forcing the truncation to 64 bit and avoiding 
      propagation of error due to floating point non conformance of certain machine/compilers.
      See -ffloat-store in gcc man page for details. */
      delta = SEIio->currAUts - SEIio->lastBPAUts;
      tmp1  = (double)delta * tc;
      buffering_period_sei.NAL_lastBPAUremoval_time = buffering_period_sei.NAL_lastBPAUremoval_time + tmp1;
      tmp1  =  (buffering_period_sei.NAL_lastBPAUremoval_time - (double) picture_timing_sei.NALfinal_arrival_time[i]) ;
      tmp2  = ((double)90000.0 * tmp1);
      buffering_period_sei.NALinitial_cpb_removal_delay[i] = (t_uint32) (tmp2 + (double)0.5);

      buffering_period_sei.NALinitial_cpb_removal_delay_offset[i] = 0xaaaaaaaa & ((1<<SEI_INITIAL_CPB_REMOVAL_DELAY_BITS)-1);
      /* NZ: NALinitial_cpb_removal_delay_offset is not used when cbr_flag = 0 (see eq. C-3 and sub);
        to avoid the emulation prevention byte a 10101... pattern is inserted */
	}
  }

  for (i=0; i < vclcnt; i++) {
      if (SEIio->InitBuffer) {
#ifdef __ndk5500_a0__
          buffering_period_sei.VCLinitial_cpb_removal_delay[i] = (t_uint32) ((double)SEIio->currAUts * (double)90000.0 * 1024 * (double)/*Gsp_h264Global*/Gsp_img->framerate_den / (double)/*Gsp_h264Global*/Gsp_img->framerate_num);
#else
          buffering_period_sei.VCLinitial_cpb_removal_delay[i] = (t_uint32) ((double)SEIio->currAUts * (double)90000.0 * 1024 / (double)/*Gsp_h264Global*/Gsp_img->framerate);
#endif
          /* NZ: VCLinitial_cpb_removal_delay_offset is not used when cbr_flag = 0 (see eq. C-3 and sub);
      to avoid the emulation prevention byte a 10101... patern is inserted */
      buffering_period_sei.VCLinitial_cpb_removal_delay_offset[i] = 0xaaaaaaaa & ((1<<SEI_INITIAL_CPB_REMOVAL_DELAY_BITS)-1);
      buffering_period_sei.VCL_lastBPAUremoval_time = (double)SEIio->currAUts * tc;
	}
    else {
      /* NZ: this chuck of code, which appear a little redundant, is so written to force
      the write of partial result to memory, forcing the truncation to 64 bit and avoiding 
      propagation of error due to floating point non conformance of certain machine/compilers.
      See -ffloat-store in gcc man page for details. */
      delta = SEIio->currAUts - SEIio->lastBPAUts;
      tmp1  = (double)delta * tc;
      buffering_period_sei.VCL_lastBPAUremoval_time = buffering_period_sei.VCL_lastBPAUremoval_time + tmp1;
      tmp1  =  (buffering_period_sei.VCL_lastBPAUremoval_time - (double) picture_timing_sei.VCLfinal_arrival_time[i]) ;
      tmp2  = ((double)90000.0 * tmp1);
      buffering_period_sei.VCLinitial_cpb_removal_delay[i] = (t_uint32) (tmp2 + (double)0.5);
      
      buffering_period_sei.VCLinitial_cpb_removal_delay_offset[i] = 0xaaaaaaaa & ((1<<SEI_INITIAL_CPB_REMOVAL_DELAY_BITS)-1);
      /* NZ: VCLinitial_cpb_removal_delay_offset is not used when cbr_flag = 0 (see eq. C-3 and sub);
        to avoid the emulation prevention byte a 10101... pattern is inserted */
    }
	}
}



/*!
 ************************************************************************
 *  \brief
 *     Convert Buffering Period SEI message data to bitstream syntax data
  ************************************************************************
 */
void FinalizeBufferingPeriod(void)
{
  CodElement codel;
  Bitstream *dest = &(buffering_period_sei.data);
  t_uint16 i, len;

  buffering_period_sei.payloadSize = 0;
  buffering_period_sei.data.bits_to_go  = 8;
  buffering_period_sei.data.byte_pos    = 0;
  buffering_period_sei.data.byte_buf    = 0;

  codel.data = buffering_period_sei.seq_parameter_set_id;
  host_writeSyntaxElement2Buf_UVLC(&codel, dest);

  if (Gsp_sps->vui_seq_parameters.nal_hrd_parameters_present_flag) {
    for (i = 0; i < (Gsp_sps->vui_seq_parameters.nal_hrd_parameters.cpb_cnt_minus1 + 1); i++) {
      len = Gsp_sps->vui_seq_parameters.nal_hrd_parameters.initial_cpb_removal_delay_length_minus1+1;
      codel.data = ((1<<len)-1) & buffering_period_sei.NALinitial_cpb_removal_delay[i];
      codel.nbit = len;
      host_writeSyntaxElement2Buf_Fixed(&codel, dest);

      len = Gsp_sps->vui_seq_parameters.nal_hrd_parameters.initial_cpb_removal_delay_length_minus1+1;
      codel.data = ((1<<len)-1) & buffering_period_sei.NALinitial_cpb_removal_delay_offset[i];
      codel.nbit = len;
      host_writeSyntaxElement2Buf_Fixed(&codel, dest);
    }
  }

  if (Gsp_sps->vui_seq_parameters.vcl_hrd_parameters_present_flag) {
    for (i=0; i< Gsp_sps->vui_seq_parameters.vcl_hrd_parameters.cpb_cnt_minus1+1; i++) {
      len = Gsp_sps->vui_seq_parameters.vcl_hrd_parameters.initial_cpb_removal_delay_length_minus1+1;
      codel.data = ((1<<len)-1) & buffering_period_sei.VCLinitial_cpb_removal_delay[i];
      codel.nbit = len;
      host_writeSyntaxElement2Buf_Fixed(&codel, dest);

      len = Gsp_sps->vui_seq_parameters.vcl_hrd_parameters.initial_cpb_removal_delay_length_minus1+1;
      codel.data = ((1<<len)-1) & buffering_period_sei.VCLinitial_cpb_removal_delay_offset[i];
      codel.nbit = len;
      host_writeSyntaxElement2Buf_Fixed(&codel, dest);
    }
  }

  /* make sure the payload is byte aligned, stuff bits are 10..0 */
  if (dest->bits_to_go != 8) {
    (dest->byte_buf) <<= 1;
    dest->byte_buf |= 1;
    dest->bits_to_go--;
    if (dest->bits_to_go != 0) (dest->byte_buf) <<= (dest->bits_to_go);
    dest->bits_to_go = 8;
    dest->streamBuffer[dest->byte_pos++]= (t_uint8) dest->byte_buf;
    dest->byte_buf = 0;
  }
  buffering_period_sei.payloadSize = dest->byte_pos;
}


/******************************/
/* PICTURE TIMING SEI MESSAGE */
/******************************/

/*!
 ************************************************************************
 *  \brief
 *     Init the global variable for Picture Timing SEI message
  ************************************************************************
 */
void InitPictureTiming()
{
    t_uint16 i;

    picture_timing_sei.cpb_removal_delay = 0xaaaaaaaa & ((1<<SEI_CPB_REMOVAL_DELAY_BITS)-1);
    picture_timing_sei.dpb_output_delay  = 0xaaaaaaaa & ((1<<SEI_DPB_REMOVAL_DELAY_BITS)-1);
    picture_timing_sei.pic_struct        = 0;
    
    for (i=0; i < MAX_NUMCLOCK_TIMESTAMP; i++)
    {
        picture_timing_sei.clock_timestamp_flag[i]	= 0xaa ;
        picture_timing_sei.ct_type[i]				= 0xaa;
        picture_timing_sei.nuit_field_based_flag[i]	= 0xaa;
        picture_timing_sei.counting_type[i]			= 0xaa;
        picture_timing_sei.full_timestamp_flag[i]	= 0xaa;
        picture_timing_sei.discountinuity_flag[i]	= 0xaa;
        picture_timing_sei.cnt_dropped_flag[i]		= 0xaa;
        picture_timing_sei.n_frames[i]				= 0xaa;
        picture_timing_sei.seconds_value[i]			= 0xaa;
        picture_timing_sei.minutes_value[i]			= 0xaa;
        picture_timing_sei.hours_value[i]			= 0xaa;
        picture_timing_sei.seconds_flag[i]			= 0xaa;
        picture_timing_sei.minutes_flag[i]			= 0xaa;
        picture_timing_sei.hours_flag[i]			= 0xaa;
        picture_timing_sei.time_offset[i]                       = 0xaa;
        picture_timing_sei.time_offset[i]			= 0xaaaaaaaa;
    }

    for (i=0; i<MAXIMUMVALUEOFcpb_cnt; i++)
    {
      picture_timing_sei.NALfinal_arrival_time[i]  = 0.0;
      picture_timing_sei.VCLfinal_arrival_time[i]  = 0.0;
    }

    picture_timing_sei.data.bits_to_go  = 8;
    picture_timing_sei.data.byte_pos    = 0;
    picture_timing_sei.data.byte_buf    = 0;
    picture_timing_sei.data.streamBuffer = &picture_timing_rbsp[0];
    for (i=0; i<PT_SEI_RBSP_BUFFER_SIZE; i++)
      picture_timing_sei.data.streamBuffer[i] = 0;
}

/*!
 ************************************************************************
 *  \brief
 *     Update global variable for Picture Timing SEI message
  ************************************************************************
 */
void UpdatePictureTiming(t_BRC_SEI* SEIio)
{
    t_uint32 deltaTS;
   
    deltaTS = SEIio->currAUts - SEIio->lastBPAUts;

    if (SEIio->forceBP)           /* Buffering Period SEI */
      SEIio->lastBPAUts = SEIio->currAUts;
    
    picture_timing_sei.cpb_removal_delay = deltaTS;
	picture_timing_sei.dpb_output_delay  = deltaTS;
}



/*!
 ************************************************************************
 *  \brief
 *     Convert Picture Timing SEI message data to bitstream syntax data
  ************************************************************************
 */
void FinalizePictureTiming()
{
  CodElement sym;
  Bitstream *dest = &(picture_timing_sei.data);
  t_uint16 i;
  picture_timing_sei.payloadSize = 0;
  picture_timing_sei.data.bits_to_go  = 8;
  picture_timing_sei.data.byte_pos    = 0;
  picture_timing_sei.data.byte_buf    = 0;

  sym.data = picture_timing_sei.cpb_removal_delay;
  /* here 
  active_sps->vui_seq_parameters.nal_hrd_parameters.cpb_removal_delay_length_minus1 
  SHALL be equals to 
  active_sps->vui_seq_parameters.vcl_hrd_parameters.cpb_removal_delay_length_minus1 
  */
  ASSERT(Gsp_sps->vui_seq_parameters.nal_hrd_parameters.cpb_removal_delay_length_minus1 == Gsp_sps->vui_seq_parameters.vcl_hrd_parameters.cpb_removal_delay_length_minus1);
  sym.nbit = Gsp_sps->vui_seq_parameters.nal_hrd_parameters.cpb_removal_delay_length_minus1 + 1;
  host_writeSyntaxElement2Buf_Fixed(&sym, dest);
  
  sym.data = picture_timing_sei.dpb_output_delay;
  /* here 
  active_sps->vui_seq_parameters.nal_hrd_parameters.dpb_output_delay_length_minus1+1
  SHALL be equals to 
  active_sps->vui_seq_parameters.vcl_hrd_parameters.dpb_output_delay_length_minus1+1
  */
  ASSERT(Gsp_sps->vui_seq_parameters.nal_hrd_parameters.dpb_output_delay_length_minus1+1 == Gsp_sps->vui_seq_parameters.vcl_hrd_parameters.dpb_output_delay_length_minus1+1);
  sym.nbit = Gsp_sps->vui_seq_parameters.nal_hrd_parameters.dpb_output_delay_length_minus1+1;
  host_writeSyntaxElement2Buf_Fixed(&sym, dest);

  if (Gsp_sps->vui_seq_parameters.pic_struct_present_flag) {
    sym.data = picture_timing_sei.pic_struct;
    sym.nbit = 4;
    host_writeSyntaxElement2Buf_Fixed(&sym, dest);

    for (i = 0; i < getNumClockTS(picture_timing_sei.pic_struct); i++) {
      sym.data = picture_timing_sei.clock_timestamp_flag[i];
      sym.nbit = 1;
      host_writeSyntaxElement2Buf_Fixed(&sym, dest);

      if (picture_timing_sei.clock_timestamp_flag[i]) {
        sym.data = picture_timing_sei.ct_type[i];
        sym.nbit = 2;
        host_writeSyntaxElement2Buf_Fixed(&sym, dest);

        sym.data = picture_timing_sei.counting_type[i];
        sym.nbit = 5;
        host_writeSyntaxElement2Buf_Fixed(&sym, dest);

        sym.data = picture_timing_sei.full_timestamp_flag[i];
        sym.nbit = 1;
        host_writeSyntaxElement2Buf_Fixed(&sym, dest);

        sym.data = picture_timing_sei.discountinuity_flag[i];
        sym.nbit = 1;
        host_writeSyntaxElement2Buf_Fixed(&sym, dest);

        sym.data = picture_timing_sei.cnt_dropped_flag[i];
        sym.nbit = 1;
        host_writeSyntaxElement2Buf_Fixed(&sym, dest);

        sym.data = picture_timing_sei.n_frames[i];
        sym.nbit = 8;
        host_writeSyntaxElement2Buf_Fixed(&sym, dest);

        if (picture_timing_sei.full_timestamp_flag[i]) {
          sym.data = picture_timing_sei.seconds_value[i];
          sym.nbit = 6;
          host_writeSyntaxElement2Buf_Fixed(&sym, dest);

          sym.data = picture_timing_sei.minutes_value[i];
          sym.nbit = 6;
          host_writeSyntaxElement2Buf_Fixed(&sym, dest);

          sym.data = picture_timing_sei.hours_value[i];
          sym.nbit = 6;
          host_writeSyntaxElement2Buf_Fixed(&sym, dest);
        } else {
          sym.data = picture_timing_sei.seconds_flag[i];
          sym.nbit = 1;
          host_writeSyntaxElement2Buf_Fixed(&sym, dest);

          if (picture_timing_sei.seconds_flag[i]) {
            sym.data = picture_timing_sei.seconds_value[i];
            sym.nbit = 6;
            host_writeSyntaxElement2Buf_Fixed(&sym, dest);

            sym.data = picture_timing_sei.minutes_flag[i];
            sym.nbit = 1;
            host_writeSyntaxElement2Buf_Fixed(&sym, dest);

            if (picture_timing_sei.minutes_flag[i]) {
              sym.data = picture_timing_sei.minutes_value[i];
              sym.nbit = 6;
              host_writeSyntaxElement2Buf_Fixed(&sym, dest);

              sym.data = picture_timing_sei.hours_flag[i];
              sym.nbit = 1;
              host_writeSyntaxElement2Buf_Fixed(&sym, dest);

              if (picture_timing_sei.hours_flag[i]) {
                sym.data = picture_timing_sei.hours_value[i];
                sym.nbit = 6;
                              host_writeSyntaxElement2Buf_Fixed(&sym, dest);
                          }
                      }
                  }
              } /* end of else */
              /* here 
              active_sps->vui_seq_parameters.nal_hrd_parameters.time_offset_length
              SHALL be equals to 
              active_sps->vui_seq_parameters.vcl_hrd_parameters.time_offset_length
              */
              ASSERT(Gsp_sps->vui_seq_parameters.nal_hrd_parameters.time_offset_length == Gsp_sps->vui_seq_parameters.vcl_hrd_parameters.time_offset_length);
        if (Gsp_sps->vui_seq_parameters.vcl_hrd_parameters.time_offset_length > 0) {
          sym.data = picture_timing_sei.time_offset[i];
                  host_writeSyntaxElement2Buf_UVLC(&sym, dest);
              }
          } /* end of if (picture_timing_sei.clock_timestamp_flag[i]) */
      } /* end of for */
  } /* end of if (active_sps->vui_seq_parameters.pic_struct_present_flag) */

  /* make sure the payload is byte aligned, stuff bits are 10..0 */
  if (dest->bits_to_go != 8) {
    (dest->byte_buf) <<= 1;
    dest->byte_buf |= 1;
    dest->bits_to_go--;
    if ( dest->bits_to_go != 0 ) (dest->byte_buf) <<= (dest->bits_to_go);
    dest->bits_to_go = 8;
    dest->streamBuffer[dest->byte_pos++]= (t_uint8) dest->byte_buf;
    dest->byte_buf = 0;
  }
  picture_timing_sei.payloadSize = dest->byte_pos;
}


/*!
 ************************************************************************
 *  \brief
 *     Support function for pic_struct in Picture Timing SEI 
 *     (not used by now) 
  ************************************************************************
 */
t_uint16 getNumClockTS(t_uint16 pic_struct) {
    switch (pic_struct)
    {
    case 0: case 1: case 2:
        return 1;
    case 3: case 4: case 7:
        return 2;
    case 5: case 6: case 8:
        return 3;
    default:
        return 0;
    }
}

#if 0
t_uint16 WriteSeiNALU() {
  /* computation of AU bits relevant to VCL NALU only and overall NALU */
  SEIio->NALAUsize = (Gsp_img->NonVCLNALUSize + /*Gs_hamac_task_info.addr_out_frame_parameters.*/Gsp_inout->bitstream_size);
  SEIio->VCLAUsize = /*Gs_hamac_task_info.addr_out_frame_parameters.*/Gsp_inout->bitstream_size;

  /* NZ: now copy the removal timestamp from hamac to SEI struct */
  SEIio->currAUts = /*Gs_hamac_task_info.ddr_out_frame_parameters.*/Gsp_inout->removal_time;

  /* set the flag InitBuffer for first input image */
  SEIio->InitBuffer = /*(Gsp_img->number==0)*/Gsp_img->FirstPictureInSequence;

#ifdef __ndk5500_a0__
  if (!skip_current) 
#else /* compute and write SEI messages in the stream */
  if (!/*Gsp_h264Global*/Gsp_inout->Skip_Current) 
#endif
  {
      WriteSEI (SEIio);
      SEIsize = 0;
  }
  return SEIsize;
}
#endif


/*!
 ************************************************************************
 * \brief
 *    Estimate the size of Buffering Period SEI message
 * \return
 *   size (in bit) of BP SEI message
 ************************************************************************
 */
t_uint16 EstimateBP(void) {
	t_uint16 size = 0;
	Bitstream b;
	t_uint8 dummy[8];

	memset((void *)&b, 0, sizeof(Bitstream));
	b.bits_to_go   = 8;
	b.streamBuffer = dummy;
	if (NAL_HRD_PARAMETERS_PRESENT_FLAG)
		size += SEI_INITIAL_CPB_REMOVAL_DELAY_BITS<<1;
	if (VCL_HRD_PARAMETERS_PRESENT_FLAG)
		size += SEI_INITIAL_CPB_REMOVAL_DELAY_BITS<<1;
	/* estimate the seq_parameter_set_id coded in variable lenght code */
	/*size += ue_v(Gsp_img->seq_parameter_set_idc, &b);*/
	size += ue_v(0, &b); /* Harcoded to 0 as for SPS */
	/* NZ: rbsp MUST be byte aligned */
	if (size%8)
		size += 8 - (size%8);
	return size;
}

/*!
 ************************************************************************
 * \brief
 *    Estimate the size of Picture Timing SEI message
 * \return
 *   size (in bit) of PT SEI message
 ************************************************************************
 */
t_uint16 EstimatePT(void) {
	t_uint16 size = 0;
	t_uint16 zero = 0;
	if (NAL_HRD_PARAMETERS_PRESENT_FLAG | VCL_HRD_PARAMETERS_PRESENT_FLAG) {
		size += SEI_CPB_REMOVAL_DELAY_BITS;
		size += SEI_DPB_REMOVAL_DELAY_BITS;
	}
	if (PIC_STRUCT_PRESENT_FLAG) {
		/* NZ: tbd */
		size += 0;
	}
	/* NZ: rbsp MUST be byte alligned */
	size += zero;//Coverity Warning Fix
	if (size%8)
		size += 8 - (size%8);
	return size;
}

//t_uint16 estimate_sei_size (void) {
t_uint16 estimate_sei_size (int forceBP) {

  /* NZ: SEI message size estimation */
  t_uint16 PTSEIsize = 0, BPSEIsize = 0;
  
  SEIsize = 0;
  /* Buffering Period + Picture Timing */
  //if (SEIio->forceBP) {
  if (forceBP) {
	  /* Estimate BP SEI size */
	  BPSEIsize = EstimateBP();
	  BPSEIsize += 8; /* NZ: +1 for last_payload_type_byte; only 1 byte because payload_type for those SEI is always less than 0xff */
	  BPSEIsize += ((BPSEIsize>>(3+8)) +1)<<3; /* NZ: an addictional 0xff byte for each byte more than 255; +1 one for last_payload_size_byte byte */
  }
  /* Picture Timing */
  /* Estimate PT SEI size */
  PTSEIsize = EstimatePT();
  PTSEIsize += 8; /* NZ: +1 for last_payload_type_byte; only 1 byte because payload_type for those SEI is always less than 0xff */
  PTSEIsize += ((PTSEIsize>>(3+8)) +1)<<3; /* NZ: an addictional 0xff byte for each byte more than 255; +1 one for last_payload_size_byte byte */    
  /* NZ
  Details:
  + 1  for final 0x80 byte in SEI payload
  + 1  for first NAL  byte : forbidden_zero_bit + nal_ref_idc + nal_unit_type
  + 4  for NAL startcode
  */
  if (1) //!Gsp_input->annexb)
	SEIsize = (PTSEIsize + BPSEIsize + ((1 + 1 + 4)<<3));
  else
    SEIsize = (PTSEIsize + BPSEIsize + ((1 + 1)<<3));
  return SEIsize;
} 
