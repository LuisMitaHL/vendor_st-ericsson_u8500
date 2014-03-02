/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef ALERT_FILTER_H_
#define ALERT_FILTER_H_

#include <grab_types.idt.h>

void alertInfoFiltered(int client_id, enum e_grabInfo info_id, t_uint16 buffer_id, t_uint16 frame_id, t_uint32 timestamp);
void alertErrorFiltered(int client_id, enum e_grabError error_id, t_uint16 data, t_uint16 buffer_id, t_uint32 timestamp);
void alertDebugFiltered(int client_id, enum e_grabDebug debug_id, t_uint16 buffer_id, t_uint16 data1, t_uint16 data2, t_uint32 timestamp);

#endif /* ALERT_FILTER_H_ */
