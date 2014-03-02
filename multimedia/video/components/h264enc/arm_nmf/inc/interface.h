/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef INTERFACE_H
#define INTERFACE_H

#include "settings.h"
#include "types.h"
#include "host_types.h"
#include "hamac_types.h"


void interface_deblocking_in(t_host_info *p_host, t_hamac_deblocking_info *p_hamac);
void interface_slice_in(t_host_info *p_host, t_hamac_slice_info *p_hamac);
void interface_concealment_in(t_uint16 mb_count, t_uint16 mb_intra, t_host_info *p_host, t_hamac_conc_info *p_hamac);
void interface_mb_in(t_hamac_slice_info *p_hamac_sl, t_hamac_mb_info *p_hamac_mb);
void interface_residual_in(t_hamac_mb_info *p_hamac_mb, t_hamac_residual_info *p_hamac_residual);

void interface_slice_out(t_uint16 *mb_count, t_uint16 *mb_intra, t_hamac_slice_info *p_hamac);
void interface_mb_out(t_hamac_mb_info *p_hamac_mb, t_hamac_slice_info *p_hamac_slice);

#endif
