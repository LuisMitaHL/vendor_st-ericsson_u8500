/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef INCLUSION_GUARD_STE_ADM_API_MODEM_H
#define INCLUSION_GUARD_STE_ADM_API_MODEM_H

#include "ste_adm_client.h"


typedef enum  {
    ADM_MODEM_STATE_UNKNOWN = 0,
    ADM_MODEM_STATE_BOOTING,
    ADM_MODEM_STATE_UPGRADING,
    ADM_MODEM_STATE_ON,
    ADM_MODEM_STATE_DUMPING,
    ADM_MODEM_STATE_OFF,
    ADM_MODEM_STATE_PREPARE_OFF,
}modem_state_t;

typedef enum  {
    ADM_MODEM_PATH_CLOSE = 0,
    ADM_MODEM_PATH_OPEN,
}modem_path_t;

ste_adm_res_t modem_init();
ste_adm_res_t modem_deinit();

modem_state_t modem_get_state();
ste_adm_res_t modem_set_samplerate(int samplerate);
ste_adm_res_t modem_configure_speech_proc(int samplerate, const char* input_dev, const char* output_dev);
ste_adm_res_t modem_set_volume(int volume);
ste_adm_res_t modem_set_tx_path(modem_path_t path_tx);
ste_adm_res_t modem_set_rx_path(modem_path_t path_rx);
ste_adm_res_t modem_reset_all_paths();
ste_adm_res_t modem_set_rec_rx_path(modem_path_t path_rec);
ste_adm_res_t modem_set_rec_tx_path(modem_path_t path_rec);
ste_adm_res_t modem_set_modem_loop(int loopback_type, int loop_enable, int codec_type);
ste_adm_res_t modem_wait_end_of_configure_speechproc_comp(void);

#endif // INCLUSION_GUARD_STE_ADM_API_MODEM_H

