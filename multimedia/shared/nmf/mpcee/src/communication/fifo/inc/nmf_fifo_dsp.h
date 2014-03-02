/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/* communication/fifo/inc/nmf_fifo_dsp.h - EE.
 *
 * Copyright (c) 2006, 2007, 2008 STMicroelectronics.
 *
 * Reproduction and Communication of this document is strictly prohibited
 * unless specifically authorized in writing by STMicroelectronics.
 * 
 * Written by NMF team.
 */
#ifndef __INC_NMF_FIFO_DSP
#define __INC_NMF_FIFO_DSP

#include <inc/type.h>
#include <share/communication/inc/nmf_fifo_desc.h>

typedef t_uword (*t_check_fct)(t_uint16 value);

t_shared_addr fifo_getAndAckNextElemToWritePointer(t_nmf_fifo fifoId);
t_shared_addr fifo_getAndAckNextElemToReadPointer(t_nmf_fifo fifoId);
void fifo_coms_acknowledgeWriteAndInterruptGeneration(t_nmf_fifo fifoId);

#endif /* __INC_NMF_FIFO_DSP */
