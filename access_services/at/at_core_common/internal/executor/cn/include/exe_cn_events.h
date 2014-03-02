/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


#ifndef __exe_cn_events_h__
#define __exe_cn_events_h__ (1)

#include <stdbool.h>

#include "cn_message_types.h"

bool exe_handle_event(cn_message_type_t type, void *data_p);

#endif /* __exe_cn_events_h__ */
