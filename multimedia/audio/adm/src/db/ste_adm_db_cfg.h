/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef INCLUSION_GUARD_STE_ADM_DB_CFG_H_
#define INCLUSION_GUARD_STE_ADM_DB_CFG_H_

#include "ste_adm_db.h"

ste_adm_res_t adm_db_cfg_create_iter(adm_db_cfg_iter_t* iter, const char* comp_name);
ste_adm_res_t adm_db_cfg_init();
ste_adm_res_t adm_db_cfg_close();

int adm_db_cfg_is_busy();
void clear_cached_config_data();


#endif // INCLUSION_GUARD_STE_ADM_DB_CFG_H_

