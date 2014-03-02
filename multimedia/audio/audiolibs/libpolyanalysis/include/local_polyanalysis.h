/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef _local_polyanalysis_h_
#define _local_polyanalysis_h_

#define HAN_SIZE 512
#define SBLIMIT   32

#include "audiolibs_common.h"
#include "polyanalysis.h"
#include "analysis_tables.h"

#define WINDOWING_NORM  (1<<(POLYANA_WINDOWING_LEFT_SHIFT))
#define MODULATION_NORM (1.0/(1<<(POLYANA_MODULATION_RIGHT_SHIFT)))

extern void
polyana_windowing(Float *z, Float *fifo_b, Float *p_y);

extern void
polyana_make_sum_diff(Float *y);

extern void
polyana_modulation(Float *s_up, Float *s_down, Float *p_sum, Float *p_diff);

extern void
polyana_make_sum_diff_scaling(Float *y, int *scale);

extern void
polyana_modulation_scaling(Float *s_up, Float *s_down, Float *p_sum, Float *p_diff, int scale);

#endif /* Do not edit below this line */

