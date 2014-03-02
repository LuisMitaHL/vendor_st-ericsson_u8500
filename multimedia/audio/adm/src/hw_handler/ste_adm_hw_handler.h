/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef STE_ADM_HW_HANDLER_H
#define STE_ADM_HW_HANDLER_H

#include <sqlite3.h>

#include "OMX_Types.h"
#include "ste_adm_client.h" // ste_adm_res_t

#include "audio_hwctrl_interface_alsa.h"

/**
* Represent the speed fade register values
* FADESPEED_FAST = 0 --> 1ms
* FADESPEED_SLOW = 3 --> 16ms
*/
typedef enum {
    FADESPEED_FAST = 0, /* 1ms */
    FADESPEED_SLOW = 3 /* 16ms */
} fadeSpeed_t;

/**
* When applying sidetone FIR-filter, this value
* sets the maximum number of tries before giving up
*/
#define MAX_TRIES_SID 5

typedef int (*hw_handler_dev_next_fp_t)(const char** dev_name_pp);
typedef int (*hw_handler_dev_to_dev_next_fp_t)(const char** src_dev_name_pp, const char** dst_dev_name_pp);
typedef ste_adm_res_t (*ste_adm_hw_handler_init_t)(const char* card_name);
typedef ste_adm_res_t (*ste_adm_hw_handler_t)(sqlite3* db_p,
                                hw_handler_dev_next_fp_t dev_next_fp,
                                hw_handler_dev_to_dev_next_fp_t dev_next_d2d_fp,
                                fadeSpeed_t fadeSpeed);
typedef ste_adm_res_t (*ste_adm_hw_handler_vc_t)(sqlite3* db_p, const char* indev, const char* outdev, int fs);

typedef struct {
    const char* card_name;
    ste_adm_hw_handler_init_t hw_handler_init;
    ste_adm_hw_handler_t hw_handler;
    ste_adm_hw_handler_vc_t hw_handler_vc;
} hw_handler_t;

enum audio_hal_chip_id_t ste_adm_hw_handler_select_hw(void);
ste_adm_res_t ste_adm_hw_handler_init(void);
ste_adm_res_t ste_adm_hw_handler(sqlite3* db_p, hw_handler_dev_next_fp_t dev_next_fp, hw_handler_dev_to_dev_next_fp_t dev_next_d2d_fp, fadeSpeed_t fadeSpeed);
ste_adm_res_t ste_adm_hw_handler_vc(sqlite3* db_p, const char* indev, const char* outdev, int fs);

#endif /* STE_ADM_HW_HANDLER_H */
