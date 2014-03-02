/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _ALGO_HPP_
#define _ALGO_HPP_

/*------------------------------------------------------------------------
 * Includes
 *----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
 * Defines
 *----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
 * Types
 *----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
 * Functions (exported)
 *----------------------------------------------------------------------*/

class mpeg4enc_arm_nmf_algo: public mpeg4enc_arm_nmf_algoTemplate
{
 // 'constructor' provided interface
  public:
    virtual t_nmf_error construct(void);

  public:
	ts_t1xhv_vec_mp4e_session Gs_vec_mp4e_session;
 

  // 'iAlgo' provided interface
  public:
    virtual void configureAlgo(
						 t_uint32 addr_in_frame_buffer,
						 t_uint32 addr_out_frame_buffer,
						 t_uint32 addr_internal_buffer,
						 t_uint32 addr_in_header_buffer,
						 t_uint32 addr_in_bitstream_buffer,
						 t_uint32 addr_out_bitstream_buffer,
						 t_uint32 addr_in_parameters,
						 t_uint32 addr_out_parameters,
						 t_uint32 addr_in_frame_parameters,
						 t_uint32 addr_out_frame_parameters);
    virtual void controlAlgo(t_t1xhv_command command, t_uint16 channelNumber, t_uint16 param);
    virtual void setDebug(t_t1xhv_debug_mode mode, t_uint32 param1, t_uint32 param2) ;

	
  public:
    mpeg4enc_arm_nmf_algo();/*: Primitive("mpeg4enc.arm_nmf.algo")*/
};
#endif /* _ALGO_H_ */

