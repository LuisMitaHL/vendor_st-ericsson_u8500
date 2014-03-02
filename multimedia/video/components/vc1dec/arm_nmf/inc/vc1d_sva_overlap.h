/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _VC1D_SVA_OVERLAP_H_
#define _VC1D_SVA_OVERLAP_H_

/*------------------------------------------------------------------------
 * Includes                                                            
 *----------------------------------------------------------------------*/
#include "vc1d_common.h"
#include "vc1d_sva.h"

/*------------------------------------------------------------------------
 * Types                                                            
 *----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
 * Functions                                                            
 *----------------------------------------------------------------------*/


void ovl_left_right_smooth(tps_macro_blk p_left, tps_macro_blk p_curr_mb);
void ovl_bottom_up_smooth(tps_macro_blk p_top_left, tps_macro_blk p_left);


#endif /* _VC1D_SVA_OVERLAP_H_ */
