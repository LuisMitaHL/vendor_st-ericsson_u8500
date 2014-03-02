/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include <inc/type.h>
//#include <inc/macros.h>

typedef struct
{
    t_sint32 nb_frames;     /**< \brief number of frames to decode   */
    char file_in[512];  /**< \brief bitstream file name to decode   */
    char file_out[512]; /**< \brief decoded file result   */
    char coverage_file[512]; /**< \brief decoded file result   */
    char batch_file[512];
    t_bool create_coverage_file;
    t_bool create_hdr_file;
    t_bool batch_mode;
    t_sint32 debug_frame_index; /**< \brief index of the frame for which to start debug   */
    t_sint32 save_frame_index;
    
} t_host_args;

int ParseArgs(int argc, char **argv, t_host_args *user_args);
t_bool read_batch_argument(FILE *p_batch_file,t_host_args *p_arguments);

#endif
