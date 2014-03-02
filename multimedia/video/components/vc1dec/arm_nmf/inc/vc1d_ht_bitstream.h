/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _VC1D_HT_BITSTREAM_H_
#define _VC1D_HT_BITSTREAM_H_

/*------------------------------------------------------------------------
 * includes                                                             
 *----------------------------------------------------------------------*/

#include "vc1d_co_bitstream.h"  /* for types definitions */

/*------------------------------------------------------------------------
 * exported functions                                                             
 *----------------------------------------------------------------------*/

t_btst_error_code       btst_init_from_file(tps_bitstream_buffer p_bitstream, 
                           char *bitstream_file_name,
                           t_uint32 byte_size);
void                    btst_end(tps_bitstream_buffer p_bitstream);
t_address               btst_get_start_address(tps_bitstream_buffer p_bitstream);
void                    btst_update_from_sva_pos(tps_bitstream_buffer p_bitstream,
                                t_address pos,t_uint32 offset);

t_btst_error_code btst_update_from_file(tps_bitstream_buffer p_bitstream, 
                                        t_uint32 byte_size);

t_uint32       btst_get_bytes_le(tps_bitstream_buffer p_bitstream,
                                          t_sint16 nb_bytes);

t_uint32 btst_show_bytes_le(tps_bitstream_buffer p_bitstream,
                            t_sint16 nb_bytes);

void btst_set_pos(tps_bitstream_buffer p_bitstream,
                  t_uint32 pos);
void btst_save_pos(tps_bitstream_buffer p_bitstream,
                           t_uint32 *p_pos);


/* duplicated for test appli */
t_btst_error_code       testapp_btst_init_from_file(tps_bitstream_buffer p_bitstream, 
                           char *bitstream_file_name,
                           t_uint32 byte_size);
void                    testapp_btst_end(tps_bitstream_buffer p_bitstream);
t_address               testapp_btst_get_start_address(tps_bitstream_buffer p_bitstream);
void                    testapp_btst_update_from_sva_pos(tps_bitstream_buffer p_bitstream,
                                t_address pos,t_uint32 offset);

t_btst_error_code testapp_btst_update_from_file(tps_bitstream_buffer p_bitstream, 
                                        t_uint32 byte_size);

t_uint32       testapp_btst_get_bytes_le(tps_bitstream_buffer p_bitstream,
                                          t_sint16 nb_bytes);

t_uint32 testapp_btst_show_bytes_le(tps_bitstream_buffer p_bitstream,
                            t_sint16 nb_bytes);

void testapp_btst_set_pos(tps_bitstream_buffer p_bitstream,
                  t_uint32 pos);
void testapp_btst_save_pos(tps_bitstream_buffer p_bitstream,
                           t_uint32 *p_pos);


#endif /* _VC1D_HT_BITSTREAM_H_ */
