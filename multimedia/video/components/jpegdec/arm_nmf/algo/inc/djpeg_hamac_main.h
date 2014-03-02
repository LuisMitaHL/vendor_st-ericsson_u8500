/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * \file 	djpeg_hamac_main.h
 * \brief 	Hamac main entry point functions declaration
 * \author  ST-Ericsson
 *  
 *  This file declares the  hamac main entry point function 
 */
/*****************************************************************************/


#ifndef _DJPEG_HAMAC_MAIN_H_
#define _DJPEG_HAMAC_MAIN_H_

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
#include "djpeg_info.h"
//#include "algo.hpp"
 /*------------------------------------------------------------------------
 * Defines							       
 *----------------------------------------------------------------------*/


/*------------------------------------------------------------------------
 * Functions (exported)
 *----------------------------------------------------------------------*/

#ifdef __NMF
extern t_decoder_state djpeg_hamac_main(tps_t1xhv_vdc_jpeg_session djpeg_session);
#else
extern t_decoder_state djpeg_hamac_main(tps_t1xhv_vdc_jpeg_session djpeg_session);
#endif

#endif /* _DJPEG_HAMAC_MAIN_H_ */
