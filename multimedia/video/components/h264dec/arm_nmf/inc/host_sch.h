/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef HOST_SCH_H
#define HOST_SCH_H

#include "types.h"
#include "hamac_types.h"


/* Threshold for sad comparison */
#define SC_TH 150000


t_uint16 SceneChangeP(t_uint16 mb_count, t_uint16 mb_intra, t_hamac_conc_info *p_hamac);
t_uint16 SceneChangeI(t_hamac_conc_info *p_hamac);

#endif
