/****************************************************************************
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \file    algo.cpp
 * \brief   jpeg Decoder algo part
 * \author  ST-Ericsson
 * 
 * 
 */
/*****************************************************************************/



#ifdef __NMF

#include <jpegdec/arm_nmf/algo.nmf>

#include <t1xhv_vdc_jpeg.idt>
#include <jpegdec.idt>
//#include "decoder.h"
//#include "algo.hpp"
//#include "djpeg_hamac_main.h"

#include "djpeg_interface.h"
//#include "djpeg_hamac_interface.h"
#include "djpeg_info.h"

#ifdef __NMF
extern "C"
{
#endif

#ifdef __NMF
extern t_decoder_state djpeg_hamac_main(tps_t1xhv_vdc_jpeg_session djpeg_session);

extern void jpegdec_get_host_parameters(tps_t1xhv_vdc_jpeg_session djpeg_session);
#else
extern t_decoder_state djpeg_hamac_main(tps_t1xhv_vdc_jpeg_session djpeg_session);

extern void void jpegdec_get_host_parameters(tps_t1xhv_vdc_jpeg_session djpeg_session);
#endif

#ifdef __NMF
}
#endif


/*------------------------------------------------------------------------
 * Global Variables
 *----------------------------------------------------------------------*/

//static t_uint32 jdcStartTime;


/*------------------------------------------------------------------------
 * Private functions prototype
 *----------------------------------------------------------------------*/
void notifyEndAlgo(t_t1xhv_status status, t_uint32 info);

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
t_nmf_error METH(construct)()
{
	return NMF_OK;
}

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
void METH(controlAlgo)(t_t1xhv_command Command, t_uint16 param)
{
    t_decoder_state retVal=DECODE_OK;
    t_t1xhv_status status=STATUS_JOB_COMPLETE;
    t_uint16 errorOrInfo = VDC_ERT_NONE;
    	
//    t_uint32 durationInTicks;
 //   t_uint32 bitstream_size;

	/* Launch jpeg Decoder */
    retVal = djpeg_hamac_main(&Gs_vdc_jpeg_session);
	
    if(retVal ==  DECODE_SUSPENDED) 
	{
	DBG_PRINTF(("suspended"));
	status = STATUS_BUFFER_NEEDED;
	}
    else if(retVal == INVALID_BITSTREAM)
	{
	DBG_PRINTF(("invalid bitstream"));
	status = STATUS_JOB_ABORTED;
	errorOrInfo = VDC_ERT_BITSTREAM_ERR;
	}	
     oEndAlgo.endAlgo(status, (t_t1xhv_decoder_info)errorOrInfo, 0);
	
  //  notifyEndAlgo(status,errorOrInfo);
	
} /* end of controlAlgo() function */


void METH(configureAlgo)( 
                    t_uint32 addr_in_frame_buffer,
                    t_uint32 addr_out_frame_buffer,
                    t_uint32 addr_internal_buffer,
                    t_uint32 addr_in_bitstream_buffer,
                    t_uint32 addr_out_bitstream_buffer,
                    t_uint32 addr_in_parameters,
                    t_uint32 addr_out_parameters,
                    t_uint32 addr_in_frame_parameters,
                    t_uint32 addr_out_frame_parameters)
{
Gs_vdc_jpeg_session.Gps_vdc_in_frame_buffer= (tps_t1xhv_vdc_frame_buf_in)addr_in_frame_buffer;
Gs_vdc_jpeg_session.Gps_vdc_out_frame_buffer = (tps_t1xhv_vdc_frame_buf_out)addr_out_frame_buffer;
Gs_vdc_jpeg_session.Gps_vdc_internal_buffer= (tps_t1xhv_vdc_internal_buf)addr_internal_buffer;
Gs_vdc_jpeg_session.Gps_vdc_in_out_bitstream_buffer = (tps_t1xhv_bitstream_buf_pos)addr_in_bitstream_buffer; 
Gs_vdc_jpeg_session.Gps_vdc_out_bitstream_buffer = (tps_t1xhv_bitstream_buf_pos)addr_out_bitstream_buffer;
Gs_vdc_jpeg_session.Gps_vdc_in_parameters = (tps_t1xhv_vdc_jpeg_param_in)addr_in_parameters;    
Gs_vdc_jpeg_session.Gps_vdc_out_parameters = (tps_t1xhv_vdc_jpeg_param_out)addr_out_parameters;   
Gs_vdc_jpeg_session.Gps_vdc_in_out_frame_parameters = (tps_t1xhv_vdc_jpeg_param_inout)addr_in_frame_parameters;
Gs_vdc_jpeg_session.Gps_vdc_out_frame_parameters = (tps_t1xhv_vdc_jpeg_param_inout)addr_out_frame_parameters; 

jpegdec_get_host_parameters(&Gs_vdc_jpeg_session);

} /* end of configureAlgo() function. */

//#pragma force_dcumode
void METH(updateAlgo)(t_t1xhv_command command,t_uint32 param1, t_uint32 param2)
{

	if(command == CMD_UPDATE_BUFFER)
	{

	     t_decoder_state retVal=DECODE_OK;
    	     t_t1xhv_status status=STATUS_JOB_COMPLETE;
   	     t_uint16 errorOrInfo = VDC_ERT_NONE;
    	



		/* Launch jpeg Decoder */
	    retVal = djpeg_hamac_main(&Gs_vdc_jpeg_session);

	if(retVal != DECODE_OK)
		{
    		if(retVal ==  DECODE_SUSPENDED) 
			{
			 status = STATUS_BUFFER_NEEDED;
			}
		else if(retVal == INVALID_BITSTREAM)
			{
			status = STATUS_JOB_ABORTED;
			errorOrInfo = VDC_ERT_BITSTREAM_ERR;
			}
		}  
   	oEndAlgo.endAlgo(status, (t_t1xhv_decoder_info)errorOrInfo, 0);	

	}


} /* End of updateAlgo() fucntion. */

void METH(setDebug)(t_t1xhv_debug_mode mode, t_uint32 param1, t_uint32 param2)
{}
/*------------------------------------------------------------------------
 * Internal Functions
 *----------------------------------------------------------------------*/
 /*****************************************************************************/
/**
 * \brief  notifyEndAlgo
 *
 * Private function to call the interface endAlgo according to the channelNumber, i.e. 
 *  instance number
 * 
  */
/*****************************************************************************/
void notifyEndAlgo (t_t1xhv_status status, t_uint32 info){
//  t_uint32  currentTimeInTicks;
//  t_uint32  durationInTicks;
  
  // Get current time.
//  currentTimeInTicks = ITC_GET_TIMER_32();
//  durationInTicks = currentTimeInTicks - jdcStartTime;
  printf("\nAlgo::calling endAlgo()");
  
//  oEndAlgo.endAlgo(status, (t_t1xhv_decoder_info)info, durationInTicks);
}  



#endif 

