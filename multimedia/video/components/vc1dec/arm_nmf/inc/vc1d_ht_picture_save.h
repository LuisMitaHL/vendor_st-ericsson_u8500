/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _VC1D_HT_PICTURE_SAVE_H_
#define _VC1D_HT_PICTURE_SAVE_H_

/*------------------------------------------------------------------------
 * Includes                                                            
 *----------------------------------------------------------------------*/

#include "vc1d_common.h" /* for tps_picture_buffer definition */

/*------------------------------------------------------------------------
 * Functions declaration                                                             
 *----------------------------------------------------------------------*/
FILE * picsv_open_picture_file(char *file_name);

//void picsv_crop_and_save_picture(FILE *p_file, tps_picture p_picture);
#ifdef __cplusplus
extern "C" 
{
#endif

void picsv_postproc_and_save_picture(FILE *p_file, tps_picture p_picture);
void picsv_init_range_red_lut(void);
#ifdef __cplusplus
}
#endif



void picsv_create_header_file(char *file_name,tps_decoder_configuration decoder_conf);



#endif /* _VC1D_HT_PICTURE_SAVE_H_ */
