/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _VC1D_SVA_RECONSTRUCTION_H_
#define _VC1D_SVA_RECONSTRUCTION_H_

/*------------------------------------------------------------------------
 * Includes                                                            
 *----------------------------------------------------------------------*/
#include "vc1d_common.h"
#include "vc1d_co_reconstruction.h"
#include "vc1d_sva.h"

/*------------------------------------------------------------------------
 * Types                                                            
 *----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
 * Functions                                                            
 *----------------------------------------------------------------------*/
void rec_initialize_reference_pointers(tps_picture_buffer p_ref_pict,
                                       tps_rec_param p_rec_param);
void rec_reconstruct_mb(t_bool is_inter_pict,
                        tps_rec_param p_rec_param,
                        tps_macro_blk p_mb,
                        tps_picture_buffer p_dest_pic);



#endif /* _VC1D_SVA_RECONSTRUCTION_H_ */
