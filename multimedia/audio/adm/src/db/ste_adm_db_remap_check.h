/*
 * Copyright (C) ST-Ericsson SA 2011. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef STE_ADM_DB_REMAP_CHECK_H_
#define STE_ADM_DB_REMAP_CHECK_H_

#include "ste_adm_client.h"

ste_adm_res_t adm_db_remap_init();
void adm_db_remap_destroy();


ste_adm_res_t adm_db_remap_verify(
    int is_input,
    const char* cur_vc_in_device,
    const char* cur_vc_out_device,
    int vc_samplerate,
    const char* cur_device,
    const char* new_device,
    const char* cur_device2,
    const char* new_device2);

#endif
