/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef STE_ADM_HW_HANDLER_U9540_H
#define STE_ADM_HW_HANDLER_U9540_H

#include <sqlite3.h>

#include "OMX_Types.h"
#include "ste_adm_client.h" // ste_adm_res_t

#include "audio_hwctrl_interface_alsa.h"

ste_adm_res_t ste_adm_hw_handler_init_u9540(const char* card_name);
ste_adm_res_t ste_adm_hw_handler_u9540(sqlite3* db_p, hw_handler_dev_next_fp_t dev_next_fp, hw_handler_dev_to_dev_next_fp_t dev_next_d2d_fp, fadeSpeed_t fadeSpeed);
ste_adm_res_t ste_adm_hw_handler_vc_u9540(sqlite3* db_p, const char* indev, const char* outdev, int fs);

#endif
