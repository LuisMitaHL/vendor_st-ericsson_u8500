/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef INCLUSION_GUARD_STE_ADM_API_TG_H
#define INCLUSION_GUARD_STE_ADM_API_TG_H

#include "ste_adm_srv.h"

#ifdef __cplusplus
  extern "C" {
#endif

void tgapi_start_tone(msg_start_comfort_tone_t* msg_p, struct srv_session_s** client_pp);
void tgapi_stop_tone(msg_base_t* msg_p, struct srv_session_s** client_pp);

#ifdef __cplusplus
  }
#endif

#endif // INCLUSION_GUARD_STE_ADM_API_TG_H
