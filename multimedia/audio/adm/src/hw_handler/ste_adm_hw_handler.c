/*
 * Copyright (C) ST-Ericsson SA 2012. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#define ADM_LOG_FILENAME "hw_handler"

#include <stdint.h>
#include <stddef.h>
#include <sqlite3.h>

#include "ste_adm_dbg.h"
#include "ste_adm_client.h" // ste_adm_res_t
#include "ste_adm_hw_handler.h"
#include "ste_adm_hw_handler_u8500.h"
#include "ste_adm_hw_handler_u9540.h"

static hw_handler_t hw_handler;

char* get_chip_id_str(enum audio_hal_chip_id_t chip_id)
{
    switch (chip_id) {
        case CHIP_ID_UNKNOWN:
            return "CHIP_ID_UNKNOWN";
	case CHIP_ID_AB8500:
            return "CHIP_ID_AB8500";
	case CHIP_ID_AB9540_V1:
            return "CHIP_ID_AB9540_V1";
	case CHIP_ID_AB9540_V2:
            return "CHIP_ID_AB9540_V2";
	case CHIP_ID_AB9540_V3:
            return "CHIP_ID_AB9540_V3";
	case CHIP_ID_AB8505_V1:
            return "CHIP_ID_AB8505_V1";
	case CHIP_ID_AB8505_V2:
            return "CHIP_ID_AB8505_V2";
	case CHIP_ID_AB8505_V3:
            return "CHIP_ID_AB8505_V3";
        default:
            return "";
    };
}
#ifdef ANDROID
enum audio_hal_chip_id_t ste_adm_hw_handler_select_hw(void)
{
    enum audio_hal_chip_id_t chip_id;

    ALOG_INFO("%s: Enter.", __func__);

    chip_id = audio_hal_alsa_get_chip_id();
    if (chip_id <= 0) {
         ALOG_INFO("%s: ERROR: Failed to get chip_id (%d)!", __func__, chip_id);
         return CHIP_ID_UNKNOWN;
    }

    switch (chip_id) {
	case CHIP_ID_AB8500:
	case CHIP_ID_AB8505_V1:
	case CHIP_ID_AB8505_V2:
	case CHIP_ID_AB8505_V3:
            hw_handler.card_name = "U85x0card";
            hw_handler.hw_handler_init = ste_adm_hw_handler_init_u8500;
            hw_handler.hw_handler = ste_adm_hw_handler_u8500;
            hw_handler.hw_handler_vc = ste_adm_hw_handler_vc_u8500;
            break;
	case CHIP_ID_AB9540_V1:
            hw_handler.card_name = "U8500card"; // This should be changed to "U85x0card" when the ASoC-driver is switched for U9540
            hw_handler.hw_handler_init = ste_adm_hw_handler_init_u9540;
            hw_handler.hw_handler = ste_adm_hw_handler_u9540;
            hw_handler.hw_handler_vc = ste_adm_hw_handler_vc_u9540;
            break;
	case CHIP_ID_AB9540_V2:
	case CHIP_ID_AB9540_V3:
            ALOG_ERR("%s: ERROR: Chip-ID '%s' not supported!", __func__, get_chip_id_str(chip_id));
            return CHIP_ID_UNKNOWN;
        case CHIP_ID_UNKNOWN:
        default:
            ALOG_ERR("%s: ERROR: Unknown codec chip-ID '%s'!", __func__, get_chip_id_str(chip_id));
            return CHIP_ID_UNKNOWN;
    };

    ALOG_INFO("%s: HW-handler initialized for '%s' (chip_id = %d).", __func__, get_chip_id_str(chip_id), chip_id);

    return chip_id;
}

ste_adm_res_t ste_adm_hw_handler_init(void)
{
    if (!hw_handler.hw_handler_init) {
         ALOG_INFO("%s: ERROR: hw_handler_init not registered!", __func__);
         return STE_ADM_RES_ALSA_ERROR;
    }
    hw_handler.hw_handler_init(hw_handler.card_name);

    return STE_ADM_RES_OK;
}

ste_adm_res_t ste_adm_hw_handler(sqlite3* db_p, hw_handler_dev_next_fp_t dev_next_fp, hw_handler_dev_to_dev_next_fp_t dev_next_d2d_fp, fadeSpeed_t fadeSpeed)
{
    if (!hw_handler.hw_handler) {
         ALOG_INFO("%s: ERROR: hw_handler not registered!", __func__);
         return STE_ADM_RES_ALSA_ERROR;
    }

    hw_handler.hw_handler(db_p, dev_next_fp, dev_next_d2d_fp, fadeSpeed);

    return STE_ADM_RES_OK;
}

ste_adm_res_t ste_adm_hw_handler_vc(sqlite3* db_p, const char* indev, const char* outdev, int fs)
{
    if (!hw_handler.hw_handler_vc) {
         ALOG_INFO("%s: ERROR: hw_handler_vc not registered!", __func__);
         return STE_ADM_RES_ALSA_ERROR;
    }

    hw_handler.hw_handler_vc(db_p, indev, outdev, fs);

    return STE_ADM_RES_OK;
}
#else //X86

ste_adm_res_t ste_adm_hw_handler_init()
{
    ste_adm_res_t res;
    extern ste_adm_res_t ste_adm_hw_handler_init_x86();
    res = ste_adm_hw_handler_init_x86();
    return res;
}

ste_adm_res_t ste_adm_hw_handler(sqlite3* db_p, hw_handler_dev_next_fp_t dev_next_fp, hw_handler_dev_to_dev_next_fp_t d2d_next_fp, fadeSpeed_t fadeSpeed)
{
    ste_adm_res_t res;
    extern ste_adm_res_t ste_adm_hw_handler_x86(sqlite3* db_p, hw_handler_dev_next_fp_t, hw_handler_dev_next_fp_t);
    res = ste_adm_hw_handler_x86(db_p, dev_next_fp, d2d_next_fp);
    return res;
}

ste_adm_res_t ste_adm_hw_handler_vc(sqlite3* db_p, const char* indev, const char* outdev, int fs)
{
    ste_adm_res_t res;
    extern ste_adm_res_t ste_adm_hw_handler_vc_x86(sqlite3* db_p, const char* indev, const char* outdev, int fs);
    res = ste_adm_hw_handler_vc_x86(db_p, indev, outdev, fs);
    return res;
}

#endif //X86
