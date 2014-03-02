/**
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __exe_cn_utils_h__
#define __exe_cn_utils_h__ (1)

#include <stdint.h>
#include <stdbool.h>

#include "exe.h"
#include "cn_data_types.h"

bool map_registration_state_from_cn_to_at(uint32_t status_cn, exe_net_reg_stat_t *status_at_p);
bool map_gprs_registration_state_from_cn_to_at(uint32_t status_cn, bool attached, exe_net_reg_stat_t *status_at_p);
bool map_rat_to_basic_act(cn_rat_type_t rat, exe_basic_act_t *act_p);
bool map_rat_to_act(cn_rat_type_t rat, exe_act_t *act_p);
int32_t exe_cn_get_exit_cause(cn_exit_cause_t* exit_cause_p);
void exe_cn_register_fail_cause(cn_exit_cause_t *exit_cause_p, const char *function_name);
bool map_cn_call_mode_to_clcc_mode(cn_call_mode_t cn_call_mode, exe_clcc_mode_t *clcc_mode_p);
bool map_cause_no_cli_to_cli_validity(cn_cause_no_cli_t cause_no_cli, const char *info_p, exe_cli_validity_t *cli_validity_p);
bool map_presentation_to_cni_validity(cn_presentation_t presentation, exe_cni_validity_t *cni_validity_p);
bool convert_cn_neighbouring_cell_info_to_exe_info(exe_2g_3g_neigh_cell_response_t *response_p, cn_neighbour_cells_info_t *neighbour_cells_info_p);

#endif /* __exe_cn_utils_h__ */
