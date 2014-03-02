/*****************************************************************************
 *  ST-Ericsson GPS Technology
 *  Copyright (C) ST-Ericsson SA 2009. All rights reserved.
 *  This code is ST-Ericsson proprietary and confidential.
 *  Any use of the code for whatever purpose is subject to
 *  specific written permission of ST-Ericsson.
 *
 *  Author: Janani Murthy
 *  Email : janani.murthy@stericsson.com
  *****************************************************************************/

#ifndef AMIL_H
#define AMIL_H

#include "agpsosa.h"

typedef bool (*t_AmilClientsCallback)(void* vp_fd, int32_t vp_SelectReturn, int8_t vp_handle);

void Amil1_01Init( void);
void Amil1_07DeRegister(int32_t vp_fd, int8_t vp_handle);
void Amil1_06RegisterForListen(int32_t vp_fd, t_AmilClientsCallback pp_DataRecvCB, int8_t vp_handle);

#endif /*AMIL_H*/
