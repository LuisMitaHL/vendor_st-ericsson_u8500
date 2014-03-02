/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */



#ifndef _VC1D_HT_SEQUENCE_HEADER_PARSER_H_
#define _VC1D_HT_SEQUENCE_HEADER_PARSER_H_
/*
 * Includes							       
 */
#include "vc1d_common.h"  /* for types definitions */
#include "OMX_Core.h"
// For ER 344032 returntype changed 
OMX_ERRORTYPE parse_sequence_header(tps_bitstream_buffer p_bitstream,
                           tps_sequence_parameters p_seq_param);


#endif /* _VC1D_HT_SEQUENCE_HEADER_PARSER_H_ */
