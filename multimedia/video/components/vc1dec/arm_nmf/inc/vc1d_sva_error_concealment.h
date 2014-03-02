/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */



#ifndef _VC1D_SVA_ERROR_CONCEALMENT_H_
#define _VC1D_SVA_ERROR_CONCEALMENT_H_


/*------------------------------------------------------------------------
 * Includes                                                            
 *----------------------------------------------------------------------*/
#include <setjmp.h>
#include "vc1d_common.h"
#include "vc1d_co_bitstream.h"

/*------------------------------------------------------------------------
 * Types                                                            
 *----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
 * Variables
 *----------------------------------------------------------------------*/

extern t_btst_error_code Global_error;
extern jmp_buf  Jump_error;
/*------------------------------------------------------------------------
 * Functions                                                            
 *----------------------------------------------------------------------*/
void  erc_handle_sync_marker(tps_bitstream_buffer p_bitstream,
                                         t_bool syncmarker_enabled,
                                         t_vc1_picture_type picture_type);


#endif /* _VC1D_SVA_ERROR_CONCEALMENT_H_ */
