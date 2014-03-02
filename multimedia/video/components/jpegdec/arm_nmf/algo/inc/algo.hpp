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


#ifndef _ALGO_HPP_
#define _ALGO_HPP_

/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/
/* #include "djpeg_interface.h"*/
#include <t1xhv_vdc_jpeg.idt>
#include <jpegdec.idt>
#include "string.h"     /* for memset function */
/*------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
 * Types
 *----------------------------------------------------------------------*/

/** \brief djpeg_hamac_main() return value   */
#if 0
typedef enum
{
    DECODE_OK=0, /**< \brief the decoding was performed well   */
    DECODE_SUSPENDED, /**< \brief the decoding was suspended because of bitstream data shortage   */
    INVALID_BITSTREAM /**< \brief the bitstream is corrupted   */

} t_decoder_state;
#endif
/*------------------------------------------------------------------------
 * Functions (exported)
 *----------------------------------------------------------------------*/
/* t_decoder_state  controlalgo(ts_t1xhv_vdc_subtask_param*   param);*/

class jpegdec_arm_nmf_algo: public jpegdec_arm_nmf_algoTemplate
{
	ts_t1xhv_vdc_jpeg_session Gs_vdc_jpeg_session;
 // 'constructor' provided interface
  public:
    virtual t_nmf_error construct(void);

  // 'iAlgo' provided interface
  public:
    virtual void configureAlgo(t_uint32 addr_in_frame_buffer, t_uint32 addr_out_frame_buffer, t_uint32 addr_internal_buffer, t_uint32 addr_in_bitstream_buffer, t_uint32 addr_out_bitstream_buffer, t_uint32 addr_in_parameters, t_uint32 addr_out_parameters, t_uint32 addr_in_frame_parameters, t_uint32 addr_out_frame_parameters);
    virtual void controlAlgo(t_t1xhv_command command, t_uint16 param);
    virtual void updateAlgo(t_t1xhv_command command,t_uint32 param1, t_uint32 param2);
    virtual void setDebug(t_t1xhv_debug_mode mode, t_uint32 param1, t_uint32 param2) ;

  // 'oEndAlgo' required interface
  public:
    //Ijpegdec_arm_nmf_api_end_algo oEndAlgo;

  public:
    jpegdec_arm_nmf_algo()/*: Primitive("jpegdec.arm_nmf.algo")*/ {
		memset(&Gs_vdc_jpeg_session,0x0,sizeof(Gs_vdc_jpeg_session));
    }
};
#endif /* _ALGO_H_ */

