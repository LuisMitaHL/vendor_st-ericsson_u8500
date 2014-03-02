/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/* rtos/trace/stm/inc/stm.h - EE.
 *
 * Copyright (c) 2006, 2007, 2008 STMicroelectronics.
 *
 * Reproduction and Communication of this document is strictly prohibited
 * unless specifically authorized in writing by STMicroelectronics.
 * 
 * Written by NMF team.
 */
#ifndef __INC_STM_H
#define __INC_STM_H

#include <inc/type.h>

#define STM_NUM_CHANNEL 256

typedef struct {
    t_shared_reg osmo[2];
    t_shared_reg osmot[2];
} t_stm_msg_reg;

typedef struct
{
    t_stm_msg_reg channel[STM_NUM_CHANNEL];
} t_stm_channel;

#endif /* __INC_STM_H */
