/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef NMF_EVENTHANDLER_H_
#define NMF_EVENTHANDLER_H_

#include "ext_hsmsig.h"
#include <cm/inc/cm_type.h>
#include "host/ext_ispctl_types.idt.h"

/* all specific event structures MUST start with "t_uint8 type;" */

struct s_NMF_ispctlInfoEvent {
    t_uint8 type;
    e_ispctlInfo info_id;
    t_uint32 value;
    ts_PageElement Listvalue[ISPCTL_SIZE_TAB_PE];
    t_uint16 number_of_pe;
    t_uint32 timestamp;
};

struct s_NMF_ispctlErrorEvent {
    t_uint8 type;
    t_uint16 error_id;
	t_uint32 last_PE_data_polled;
	t_uint16 number_of_pe;
    t_uint32 timestamp;
};

struct s_NMF_ispctlDebugEvent {
    t_uint8 type;
    t_uint16 debug_id;
    t_uint16 data1;
    t_uint16 data2;
    t_uint32 timestamp;
};

struct s_NMF_tuningLoaderEvent {
    t_uint8 type;
    t_sint32 error;
};

struct s_NMF_grabInfoEvent {
    t_uint8 type;
    t_uint16 info_id;
    t_uint16 buffer_id;
    t_uint32 stabvect_x;
    t_uint32 stabvect_y;
    t_uint32 timestamp;
};

struct s_NMF_grabErrorEvent {
    t_uint8 type;
    t_uint16 error_id;
    t_uint16 buffer_id;
    t_uint32 timestamp;
};

struct s_NMF_grabDebugEvent {
    t_uint8 type;
    t_uint16 debug_id;
    t_uint16 buffer_id;
    t_uint16 data1;
    t_uint16 data2;
    t_uint32 timestamp;
};



#endif /*NMF_EVENTHANDLER_H_*/
