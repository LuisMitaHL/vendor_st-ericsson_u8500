/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/* communication/inc/communication.h - EE.
 *
 * Copyright (c) 2006, 2007, 2008 STMicroelectronics.
 *
 * Reproduction and Communication of this document is strictly prohibited
 * unless specifically authorized in writing by STMicroelectronics.
 * 
 * Written by NMF team.
 */
#ifndef __INC_NMF_COMMUNICATION
#define __INC_NMF_COMMUNICATION

#include <inc/type.h>
#include <communication/api/descriptor.idt>

void PushEventTace(t_nmf_fifo fifoId, t_event_params_handle h, t_uword id, t_uint32 isTrace);

#endif /* __INC_NMF_COMMUNICATION */
