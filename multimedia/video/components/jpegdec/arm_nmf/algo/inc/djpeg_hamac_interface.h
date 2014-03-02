/****************************************************************************
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \file 	djpeg_hamac_interface.h
 * \brief 	interfacing functions declarartion from hamac locals 
 *              informations to host interface 
 * \author  ST-Ericsson
 *  
  *  
 *  This file declares interfacing functions on host side. Those functions 
 *  either reconstruct Hamac_info from jpeg_param_(in)out or construct 
 *  jpeg_param_out , jpeg_param_inout from Hamac_info
 *  Note that those function use directly global variables declared either on
 *  host side or on hamac side. The extern keyword is used to "indicate" from
 *  where the variable are "coming"
 * 
 */
/*****************************************************************************/

#ifndef _DJPEG_HAMAC_INTERFACE_
#define _DJPEG_HAMAC_INTERFACE_

/*------------------------------------------------------------------------
 * Includes							       
 *----------------------------------------------------------------------*/
#ifdef __NMF
#include <t1xhv_vdc_jpeg.idt>
#include <jpegdec.idt>
#else
#include "djpeg_interface.h"
#include "djpeg_global.h"
#endif
/*------------------------------------------------------------------------
 * Defines							       
 *----------------------------------------------------------------------*/
/*------------------------------------------------------------------------
 * Variables
 *----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
 * Functions (exported)
 *----------------------------------------------------------------------*/
extern void jpegdec_get_host_parameters(tps_t1xhv_vdc_jpeg_session djpeg_session); 

void jpegdec_set_host_parameters(tps_t1xhv_vdc_jpeg_session djpeg_session); 

//void  updateBitstreamInfo(t_uint32 param1,t_uint32 param2);

#endif /* _DJPEG_HAMAC_INTERFACE_ */
