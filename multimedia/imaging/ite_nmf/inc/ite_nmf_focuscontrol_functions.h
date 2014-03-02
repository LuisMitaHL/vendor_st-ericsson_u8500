/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#ifndef ITE_NMF_FOCUSCONTROL_FUNCTIONS_H_
#define ITE_NMF_FOCUSCONTROL_FUNCTIONS_H_


#include "VhcElementDefs.h"
#include "ite_testenv_utils.h"
#include "ite_sia_buffer.h"
#include "grab_types.idt.h"
#include "ite_main.h"
#include "cli.h"

typedef struct
{
    //statistic of each zone
    AFStats_HostZoneStatus_ts zone[10];
    t_uint32 bo_v;
    t_uint32 len;
}AFStats_Statistics_ts;

#endif /*ITE_NMF_FOCUSCONTROL_FUNCTIONS_H_*/
