/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef t_stamps_h
#define t_stamps_h


typedef struct reset_ind {
    long int          rtime;
    struct reset_ind *next;
}  reset_ind_s;


typedef struct info_reset {
    reset_ind_s *p_start;      /* fist element list    */
    reset_ind_s *list;         /* Current element list */
    int          total_reset_number; /* number of reset registered */
} info_reset_s;


info_reset_s * list_create_reset(void);
int store_reset_ind(info_reset_s *, int , int);


#endif /* t_stamps_h */
