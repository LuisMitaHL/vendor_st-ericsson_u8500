/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef STE_ADM_CLIENT_UTIL_INCLUSION_GUARD_H
#define STE_ADM_CLIENT_UTIL_INCLUSION_GUARD_H

int adm_connect_and_exchange_cmd(void* cmd_base);
int adm_exchange_cmd(int fd, void* cmd_base);

#endif
