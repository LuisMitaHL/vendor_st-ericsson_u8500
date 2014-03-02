/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _VC1D_CO_RECONSTRUCTION_H_
#define _VC1D_CO_RECONSTRUCTION_H_





/*------------------------------------------------------------------------
 * Includes                                                            
 *----------------------------------------------------------------------*/
#include "vc1d_common.h"

/*------------------------------------------------------------------------
 * Defines                                                            
 *----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
 * Types                                                            
 *----------------------------------------------------------------------*/



/*------------------------------------------------------------------------
 * Functions                                                            
 *----------------------------------------------------------------------*/

void rec_evaluate_rec_param_from_picture_dimension(t_uint16 width,
                                                   t_uint16 height,
                                                   tps_rec_param p_rec_param);

void rec_initialize_destination_pointers(tps_picture_buffer p_dest_pict,
                                                tps_rec_param p_rec_param);

/* MC: dupl declarations for arm-side */
void arm_rec_evaluate_rec_param_from_picture_dimension(t_uint16 width,
                                                   t_uint16 height,
                                                   tps_rec_param p_rec_param);

void arm_rec_initialize_destination_pointers(tps_picture_buffer p_dest_pict,
                                                tps_rec_param p_rec_param);


/* MC: dupl declarations for arm-side */
void testapp_arm_rec_evaluate_rec_param_from_picture_dimension(t_uint16 width,
                                                   t_uint16 height,
                                                   tps_rec_param p_rec_param);

void testapp_arm_rec_initialize_destination_pointers(tps_picture_buffer p_dest_pict,
                                                tps_rec_param p_rec_param);



#endif /* _VC1D_CO_RECONSTRUCTION_H_ */
