/*
* Copyright (C) ST-Ericsson SA 2010. All rights reserved.
* This code is ST-Ericsson proprietary and confidential.
* Any use of the code for whatever purpose is subject to
* specific written permission of ST-Ericsson SA.
*/
#ifdef __NMF
#include <stdlib.h>
#include <mpeg4enc/arm_nmf/algo.nmf>
#endif


#if 1
static void memcpy (void * destination, const void * source, t_uint32 num)
{
	t_uint32 *src, *dest;
	t_uint32 i;

	dest = (t_uint32 *)destination;
	src  = (t_uint32 *)source;

	for (i=0; i<num/4; i++)
	{
		*dest = *src;

		dest++;
		src++;
	}
}

/**
static void memset (void * destination, t_uint32 size, t_uint32 val)
{
	t_uint32 i;
	t_uint8 *dest;

	dest = (t_uint8 *)destination;

	for (i=0; i<size; i++)
	{
		*dest = val;
		dest++;
	}
}**/
#endif

/*****************************************************************************/
/**
  * \brief  constructor
  *
  * MPEG4 Algo class constructor
  *		
  */
mpeg4enc_arm_nmf_algo::mpeg4enc_arm_nmf_algo()
{
	Gs_vec_mp4e_session.G_addr_out_frame_parameters = NULL;
	Gs_vec_mp4e_session.G_addr_out_parameters = NULL;
	Gs_vec_mp4e_session.Gps_bitstream_buf_header  = NULL;
	Gs_vec_mp4e_session.Gps_bitstream_buf_pos = NULL;
	Gs_vec_mp4e_session.Gps_bitstream_buf_pos_out  = NULL;
	Gs_vec_mp4e_session.Gps_vec_frame_buf_in  = NULL;
	Gs_vec_mp4e_session.Gps_vec_frame_buf_out  = NULL;
	Gs_vec_mp4e_session.Gps_vec_internal_buf  = NULL;
//	Gs_vec_mp4e_session.Gs_vec_mp4e_global  = NULL;
	Gs_vec_mp4e_session.Gps_vec_mpeg4_param_in = NULL;
	Gs_vec_mp4e_session.Gps_vec_mpeg4_param_inout  = NULL;
	Gs_vec_mp4e_session.Gps_vec_mpeg4_param_out  = NULL;
	Gs_vec_mp4e_session.QP[0] = 0;
	Gs_vec_mp4e_session.not_coded_P3[0]  = 0;
	Gs_vec_mp4e_session.slice_num[0]  = 0;
} /* End of mpeg4enc_arm_nmf_algo() CONSTRUCTOR. */



  /*****************************************************************************/
  /**
  * \brief  init
  *
  * Init MPEG4 Algo encode component. Implicitly called while instanciating
  * 	this component.
  * 	Unmask wanted internal interrupts.
  */
  /*****************************************************************************/
  t_nmf_error mpeg4enc_arm_nmf_algo::construct(void)
  {
	  return NMF_OK;
  } /* End of init() function. */
  
  
  
  /*****************************************************************************/
  /**
  * \brief  controlAlgo
  *
  * Control MPEG4 Algo encode component. reserved for future use.
  *
  * \param   sCommand
  * \param   param (n/a)
  */
  /*****************************************************************************/

extern "C" t_uint32 mp4e_encode_frame(tps_t1xhv_vec_mp4e_session ptr_session);


  void mpeg4enc_arm_nmf_algo::controlAlgo(t_t1xhv_command command, t_uint16 channelNumber, t_uint16 param)
  {
	  t_uint32 skipcur;
//	  tps_t1xhv_vec_mpeg4_param_inout inout;

	  switch(command)
	  {
	  case 0:
	  case 1:
		  /* (Not yet implemented) */
		  break;

	  case 2:
		  skipcur = mp4e_encode_frame(&Gs_vec_mp4e_session);
		  Gs_vec_mp4e_session.Gps_vec_mpeg4_param_inout->bitstream_size = Gs_vec_mp4e_session.Gs_vec_mp4e_global.WriteBits.bitstream.buffer_bits;
		  Gs_vec_mp4e_session.Gps_vec_mpeg4_param_inout->bitstream_size = Gs_vec_mp4e_session.Gps_vec_mpeg4_param_inout->bitstream_size
			  - Gs_vec_mp4e_session.Gps_vec_mpeg4_param_inout->stuffing_bits;
		  Gs_vec_mp4e_session.Gps_vec_mpeg4_param_inout->Skip_Current   = skipcur;
		  Gs_vec_mp4e_session.Gps_vec_mpeg4_param_inout->hec_count   = Gs_vec_mp4e_session.Gs_vec_mp4e_global.Bitstream.hec_count;
		 //> Gs_vec_mp4e_session.Gps_vec_mpeg4_param_inout->pictCount   = Gs_vec_mp4e_session.Gps_vec_mpeg4_param_inout->pictCount;

		  //should minimize this
		  memcpy((void *)Gs_vec_mp4e_session.G_addr_out_frame_parameters, (void *)Gs_vec_mp4e_session.Gps_vec_mpeg4_param_inout, 	sizeof(ts_t1xhv_vec_mpeg4_param_inout));
//		  inout = (tps_t1xhv_vec_mpeg4_param_inout)Gs_vec_mp4e_session.G_addr_out_frame_parameters;
//		  inout->bitstream_size = Gs_vec_mp4e_session.Gps_vec_mpeg4_param_inout->bitstream_size;
//		  inout->Skip_Current = Gs_vec_mp4e_session.Gps_vec_mpeg4_param_inout->Skip_Current;
//		  inout->hec_count = Gs_vec_mp4e_session.Gps_vec_mpeg4_param_inout->hec_count;

		  //Dipti 24.08.2011
		  //oEndAlgo.endAlgo((t_t1xhv_status)0, (t_t1xhv_encoder_info)STATUS_JOB_COMPLETE,skipcur);
		  oEndAlgo.endAlgo((t_t1xhv_status)STATUS_JOB_COMPLETE, (t_t1xhv_encoder_info)(skipcur? 2 : 0),0);

		  break;
	  default:
		  break;
	  } /* switch (Command) */

} /* end of controlAlgo() function */

/*****************************************************************************/
/**
* \brief  configureAlgo
*
* Configure MPEG4 Algo encode component adresses of all required data
*  structures
*/
/*****************************************************************************/
void mpeg4enc_arm_nmf_algo::configureAlgo(
											 t_uint32 addr_in_frame_buffer,
											 t_uint32 addr_out_frame_buffer,
											 t_uint32 addr_internal_buffer,
											 t_uint32 addr_in_header_buffer,
											 t_uint32 addr_in_bitstream_buffer,
											 t_uint32 addr_out_bitstream_buffer,
											 t_uint32 addr_in_parameters,
											 t_uint32 addr_out_parameters,
											 t_uint32 addr_in_frame_parameters,
											 t_uint32 addr_out_frame_parameters
										)
{

		Gs_vec_mp4e_session.G_addr_out_frame_parameters = (t_uint32)addr_out_frame_parameters;
		Gs_vec_mp4e_session.Gps_vec_mpeg4_param_out	= (tps_t1xhv_vec_mpeg4_param_out)addr_in_frame_parameters;
		
		/* Get relevant field from the addresses given as parameter */
		Gs_vec_mp4e_session.Gps_vec_frame_buf_in	= (tps_t1xhv_vec_frame_buf_in)addr_in_frame_buffer;
		Gs_vec_mp4e_session.Gps_vec_frame_buf_out	= (tps_t1xhv_vec_frame_buf_out)addr_out_frame_buffer;
		Gs_vec_mp4e_session.Gps_vec_internal_buf	= (tps_t1xhv_vec_internal_buf)addr_internal_buffer;
		Gs_vec_mp4e_session.Gps_bitstream_buf_header	= (tps_t1xhv_vec_header_buf)addr_in_header_buffer;
		Gs_vec_mp4e_session.Gps_bitstream_buf_pos		= (tps_t1xhv_bitstream_buf_pos)addr_in_bitstream_buffer;
		Gs_vec_mp4e_session.Gps_vec_mpeg4_param_in		= (tps_t1xhv_vec_mpeg4_param_in)addr_in_parameters;
		Gs_vec_mp4e_session.Gps_vec_mpeg4_param_inout	= (tps_t1xhv_vec_mpeg4_param_inout)addr_in_frame_parameters;

		//configureAlgo API modification needed
		//should avoid this
		//corresponding changes made in t1xhv_vec_mpeg4.idt
//		Gs_vec_mp4e_session.Gps_vec_mp4e_global = (tps_t1xhv_vec_mp4e_global)malloc(1*sizeof(ts_t1xhv_vec_mp4e_global));

} /* end of configureAlgo() function. */




/*****************************************************************************/
/**
* \brief  setDebug
*
* Set the debug mode of the MPEG4 encoder NMf component
*
* \param	mode.
* \param	param1.
* \param	param2.
*/
/*****************************************************************************/
void mpeg4enc_arm_nmf_algo::setDebug(t_t1xhv_debug_mode mode, t_uint32 param1, t_uint32 param2)
{
//	printf("mpeg4enc_sw_arm_nmf_algo::setDebug\n");
} /* End of setDebug() fucntion. */

  /*------------------------------------------------------------------------
  * Internal Functions
*----------------------------------------------------------------------*/
