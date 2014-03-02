/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 * Author: Jens Wiklander <jens.wiklander@stericsson.com>
 *  for ST-Ericsson
 */

#ifndef __AVS_CALC_H__
#define __AVS_CALC_H__

#include <types.h>

void avs_calculate_settings(u32 avs_fuses[2], void *avs_data);
void no_avs_calculate_settings(u32 avs_fuses[2], void *avs_data);

int has_avs(void);

int is_fused(u32 avs_fuses[2]);
int is_maxopp_fused(u32 avs_fuses[2]);
void avs_init(char* build_date, u32 bin, u32 bin_len);

#endif /*__AVS_CALC_H__*/
