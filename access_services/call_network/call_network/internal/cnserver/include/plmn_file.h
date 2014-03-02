/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __plmn_file_h__
#define __plmn_file_h__ (1)

#include "plmn_list.h"


int plmn_file_load(const plmn_list_source_t source, const char *filepath_p);
int plmn_file_dump(const plmn_list_source_t source, const char *filepath_p);

#endif /* __plmn_file_h__ */
