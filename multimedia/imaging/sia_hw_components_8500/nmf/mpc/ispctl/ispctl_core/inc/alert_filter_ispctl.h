/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
/*
* \brief    functions declaration file
*/
 /** \file    alert_filter_ispctl.c */


#ifndef ALERT_FILTER_H_
#define ALERT_FILTER_H_

#include <ispctl_types.idt.h>

void alertInfoFiltered(enum e_ispctlInfo info_id, t_uint32 value, t_uint32 timestamp);
void alertInfoListFiltered(enum e_ispctlInfo info_id, ts_PageElement listvalue[ISPCTL_SIZE_TAB_PE], t_uint16 nb_of_pe, t_uint32 timestamp);
void alertErrorFiltered(enum e_ispctlError error_id, t_uint32 data, t_uint32 timestamp);
void alertDebugFiltered(enum e_ispctlDebug debug_id, t_uint16 data1, t_uint16 data2, t_uint32 timestamp);

#endif /* ALERT_FILTER_H_ */
