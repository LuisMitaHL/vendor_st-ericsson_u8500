/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*
 * \file    cmal.h
 * \brief   STE 3A CMAL (camera module abstraction layer) reference library interface
 * \author  ST-Ericsson Image Quality Team, Agrate (Italy)
 */

#ifndef CMAL_H_
#define CMAL_H_


#include "ste3a.h"

ste3acb_cmal_closest_ET ste3a_cmal_ref_closest_ET;
ste3acb_cmal_closest_IT ste3a_cmal_ref_closest_IT;
ste3acb_cmal_closest_AG ste3a_cmal_ref_closest_AG;
ste3acb_cmal_closest_IR ste3a_cmal_ref_closest_IR;
ste3acb_cmal_closest_NF ste3a_cmal_ref_closest_NF;

ste3acb_cmal_set_LA_model  ste3a_cmal_ref_set_LA_model;
ste3acb_cmal_convert_LA_ln ste3a_cmal_ref_convert_LA_ln;
ste3acb_cmal_convert_LA_nl ste3a_cmal_ref_convert_LA_nl;
ste3acb_cmal_range_LA_ninf ste3a_cmal_ref_range_LA_ninf;
ste3acb_cmal_range_LA_nmac ste3a_cmal_ref_range_LA_nmac;

void cmal_ref_set_ET_model(ste3a_time min, ste3a_time max, ste3a_time base, ste3a_time step);

void cmal_ref_set_AG_model(ste3a_gain min, ste3a_gain max, ste3a_gain base, ste3a_gain step);

void cmal_ref_set_AG_model(ste3a_float num_pre_mul, ste3a_float num_post_add, ste3a_float den_pre_mul, ste3a_float den_post_add, ste3a_index min_code, ste3a_index max_code);

#endif /* CMAL_H_ */
