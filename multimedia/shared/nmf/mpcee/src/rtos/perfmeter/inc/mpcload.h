/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/* rtos/perfmeter/inc/mpcload.h - EE.
 *
 * Copyright (c) 2006, 2007, 2008 STMicroelectronics.
 *
 * Reproduction and Communication of this document is strictly prohibited
 * unless specifically authorized in writing by STMicroelectronics.
 * 
 * Written by NMF team.
 */
/*
 *
 */
#ifndef MPC_LOAD_H
#define MPC_LOAD_H

void mpcload_SetIdleState(void);
void mpcload_SetLoadState(void);
t_uint56 mpcload_GetCurrentTimerValue();

#endif /* MPC_LOAD_H */
