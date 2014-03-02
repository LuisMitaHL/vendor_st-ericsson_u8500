#ifndef INCLUSION_GUARD_SIM_STUB_H
#define INCLUSION_GUARD_SIM_STUB_H
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#include <stdint.h>
#include "sim_internal.h"

#define SIM_SOCKET_PATH                     "/tmp/socket_sim"
#define SIMSTUB_SOCKET_PATH                 "/tmp/socket_sim_stub"

#define SIM_READER_INPUT_BUFFER_SIZE        (2048)

#define STE_SIM_REQ_CONNECT                 (STE_SIM_RANGE_REQ | 0x0005)
#define STE_SIM_REQ_DISCONNECT              (STE_SIM_RANGE_REQ | 0x0006)
#define STE_SIM_REQ_HANGUP                  (STE_SIM_RANGE_REQ | 0x0007)

// SIM Get State REQUEST
#define STE_UICC_REQ_GET_SIM_STATE          (STE_SIM_RANGE_REQ | 0x0008)

// SIM Envelope Command REQUEST
#define STE_CAT_REQ_EC                      (STE_SIM_RANGE_REQ | 0x0009)

#endif                          // INCLUSION_GUARD_SIM_STUB_H
