/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/* semaphores/inc/semaphores.h - EE.
 *
 * Copyright (c) 2006, 2007, 2008 STMicroelectronics.
 *
 * Reproduction and Communication of this document is strictly prohibited
 * unless specifically authorized in writing by STMicroelectronics.
 * 
 * Written by NMF team.
 */
#ifndef __INC_NMF_DSP_SEMAPHORE_H
#define __INC_NMF_DSP_SEMAPHORE_H

#include <share/semaphores/inc/semaphores.h>

void sem_Init(t_uword usedIntLevel);
void sem_GenerateIrq(t_semaphore_id semId);
t_nmf_core_id sem_GetFromCoreIdFromIrqSrc(void);
t_nmf_core_id sem_GetToCoreIdFromSemId(t_semaphore_id semId);

#endif /* __INC_NMF_DSP_SEMAPHORE_H */
