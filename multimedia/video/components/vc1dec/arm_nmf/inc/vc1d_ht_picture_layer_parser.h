/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _VC1D_HT_PICTURE_LAYER_PARSER_
#define _VC1D_HT_PICTURE_LAYER_PARSER_


/*------------------------------------------------------------------------
 * Includes                                                            
 *----------------------------------------------------------------------*/
#include "vc1d_common.h"
#include "vc1d_ht_rcv_parser.h" /* for decoder_conf struct */

/*------------------------------------------------------------------------
 * Types                                                            
 *----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
 * Functions                                                            
 *----------------------------------------------------------------------*/

t_btst_error_code btpar_parse_picture_type(tps_bitstream_buffer p_bitstream,
                                           tps_sequence_parameters p_seq_param,
                                           t_vc1_picture_type *p_pict_type);


#endif /* _VC1D_HT_PICTURE_LAYER_PARSER_ */


