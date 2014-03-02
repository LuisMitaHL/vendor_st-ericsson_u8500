/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


/*------------------------------------------------------------------------
 * Includes                                                            
 *----------------------------------------------------------------------*/
#include "vc1d_common.h"
#include "vc1d_co_reconstruction.h"
/*------------------------------------------------------------------------
 * Defines                                                            
 *----------------------------------------------------------------------*/

#if defined(PRINT_RECONSTRUCTION) || defined(PRINT_ALL)
#define PRINTF(value) if (Dbg) printf value /**< \brief debug printf function: use double (( when calling   */
#else
#define PRINTF(value)
#endif

/*------------------------------------------------------------------------
 *  functions                                                            
 *----------------------------------------------------------------------*/

void arm_rec_evaluate_rec_param_from_picture_dimension(t_uint16 width,
                                                   t_uint16 height,
                                                   tps_rec_param p_rec_param)
{

    
    p_rec_param->y_width = width;
    p_rec_param->padded_y_width = EXTEND_TO_16(width);
    p_rec_param->y_height = height;
    p_rec_param->padded_y_height = EXTEND_TO_16(height);

    p_rec_param->cbcr_width = width/2;
    p_rec_param->padded_cbcr_width = EXTEND_TO_8(width/2);
    p_rec_param->cbcr_height = height/2;
    p_rec_param->padded_cbcr_height = EXTEND_TO_8(height/2);


    PRINTF(("\t y width = %d, padded to MB y width = %d\n\t y height = %d padded to MB y height = %d\n",
                p_rec_param->y_width,p_rec_param->padded_y_width,
                p_rec_param->y_height,p_rec_param->padded_y_height));

    PRINTF(("\t cbcr width = %d, padded to MB cbcr width = %d\n\t cbcr height = %d padded to MB cbcr height = %d\n",
                p_rec_param->cbcr_width,p_rec_param->padded_cbcr_width,
                p_rec_param->cbcr_height,p_rec_param->padded_cbcr_height));
            
    
}

void arm_rec_initialize_destination_pointers(tps_picture_buffer p_dest_pict,
                                               tps_rec_param p_rec_param)
{
    t_uint32 y_size;
    t_uint32 cbcr_size;
    PRINTF(("Evaluating picture buffer pointers\n"));
    y_size = (t_uint32)p_rec_param->padded_y_width * (t_uint32)p_rec_param->padded_y_height;
    cbcr_size = (t_uint32)p_rec_param->padded_cbcr_width * (t_uint32)p_rec_param->padded_cbcr_height;
    p_dest_pict->y_size    = y_size;
    p_dest_pict->cb_size = cbcr_size;
    p_dest_pict->cr_size = cbcr_size;

    /* We update set the pointers in the destination buffer: */
#ifdef DSP_BUILD
    p_dest_pict->cb = p_dest_pict->y + y_size/2;
//        + (t_uint32)p_rec_param->padded_y_width * (t_uint32)p_rec_param->padded_y_height/2;
    p_dest_pict->cr = p_dest_pict->cb + cbcr_size/2;
//        + (t_uint32)p_rec_param->padded_cbcr_width * (t_uint32)p_rec_param->padded_cbcr_height/2;

#else
    p_dest_pict->cb = p_dest_pict->y + y_size;
//        + (t_uint32)p_rec_param->padded_y_width * (t_uint32)p_rec_param->padded_y_height;
    p_dest_pict->cr = p_dest_pict->cb + cbcr_size;
//        + (t_uint32)p_rec_param->padded_cbcr_width * (t_uint32)p_rec_param->padded_cbcr_height;

#endif



    PRINTF(("\t y stored  at @ 0x%lp (size = %u)\n",p_dest_pict->y,p_dest_pict->y_size));
    PRINTF(("\t cb stored at @ 0x%lp (size = %u)\n",p_dest_pict->cb,p_dest_pict->cb_size));
    PRINTF(("\t cr stored at @ 0x%lp (size = %u)\n",p_dest_pict->cr,p_dest_pict->cr_size));

}
                                        
