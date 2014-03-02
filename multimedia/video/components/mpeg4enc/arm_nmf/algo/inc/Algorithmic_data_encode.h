/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef ALGORITHMIC_DATA_ENCODE_H
#define ALGORITHMIC_DATA_ENCODE_H

#include <stdio.h>
#include "motion_emp.h"

typedef struct
{
  t_sint8 *air_timers;                /* FP: the attribute static is removed to implement the CIR fix */
  t_sint8 *air_indexes;
}AIR_Data;
/* Function prototypes */

void mp4e_Algorithmic_EncodePicture(unsigned char  *input_frame,
             unsigned char  *recon_frame,
             unsigned char  *aux_frame,
             mp4_parameters *mp4_par,
             short         param,
             int         inter_picture,
             int         skipcount,
             int         change_frame_id,
             int         *gob_frame_id,
             ui16_t      *gui16_inserted_gob,
             struct      time_stamp_t *ts,
             AIR_Data    *Air_p);

             
void mp4e_Algorithmic_EncodePicture_VR(unsigned char  *input_frame,
                                 unsigned char  *recon_frame,
                                 unsigned char  *aux_frame,
                                 mp4_parameters *mp4_par,
                                 short          param,
                                 int            inter_picture,
                                 int            skipcount,
                                 struct time_stamp_t *ts);
                                 
void mp4e_algorithmic_AIR_shutdown(AIR_Data *Air_p);

#endif
