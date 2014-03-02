/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   nmf_mutex.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/

#ifndef NMF_MUTEX_H_
#define NMF_MUTEX_H_

#include "ENS_Redefine_Class.h"

/* WARNING : Theses functions are for internal ENS use ONLY !!
 * -----------------------------------------------------------
 * The need is to implement "user-fifo", which is mandatory to implement an OMX behaviour.
 * 
 * Others Components have to use NMF asynchronous bindings and the "run-to-completion" 
 * scheme to manage SMP issue.
 */


/* WARNING : It breaks each time the NMF Team reworks EE.
 * This API should be the same for each EE soon.
 */
/* NOTE : We have to inform NMF Team if this hook is not used anymore */ 


#define NMF_mutex_create	eeMutexCreate
#define NMF_mutex_destroy	eeMutexDestroy

#define NMF_mutex_lock		eeMutexLock
#define NMF_mutex_lockTry	eeMutexLockTry
#define NMF_mutex_unlock	eeMutexUnlock

#include <inc/typedef.h>

// Hook to EE 'mutex' interface
extern "C" {

/* WARNING: this type must be aligned with NMF's internal hMutex type! It may change
   on new releases */
typedef t_uint32 hMutex;

ENS_API_IMPORT hMutex   eeMutexCreate();
ENS_API_IMPORT t_sint32 eeMutexDestroy(hMutex mutex);
ENS_API_IMPORT void     eeMutexLock(hMutex mutex);
ENS_API_IMPORT t_sint32 eeMutexLockTry(hMutex mutex);
ENS_API_IMPORT void     eeMutexUnlock(hMutex mutex); 

} /* extern "C" */

#endif /* NMF_MUTEX_H_ */
