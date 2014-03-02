/*=============================================================================
 * Copyright (C) ST-Ericsson SA 2010
 * Author: wlan_humac.com
 * License terms: GNU General Public License (GPL), version 2.
 *===========================================================================*/
/**
 * \addtogroup OS_Interface_Layer
 * \brief
 * This interface defines all the services UMAC implementation expects from
 * the operating system/platform.  This includes Timer services, Memory
 * Services, Debug API's, Endianness conversion macros etc.
 *
 */
/**
 * \file os_if.h
 * - <b>PROJECT</b>		: WLAN HOST UMAC
 * - <b>FILE</b>		: os_if.h
 * \brief
 * This header file defines the interface to core UMAC Component.
 * \ingroup OS_Interface_Layer
 * \date 04/10/08 18:28
 */
#ifndef _OS_IF_H
#define _OS_IF_H
/******************************************************************************
				INCLUDE FILES
******************************************************************************/

#include "stddefs.h"
#include "UMI_OsIf.h"


/******************************************************************************
			PLATFORM MAPPING
******************************************************************************/

typedef void *OS_LOCK_TYPE;
/* Definations for UMAC core */
#define OS_INIT_TIMER(pTimer, pCallback)
#define OS_START_TIMER(pTimer, Time, arg)	UMI_StartTimer(pTimer, Time, arg)
#define OS_STOP_TIMER(pTimer)			UMI_StopTimer(pTimer)
#define OS_CREATE_TIMER(TimerObject, TimeOutCb) UMI_CreateTimer(TimeOutCb)
#define OS_DESTROY_TIMER(TimerHandle)		UMI_DestroyTimer(TimerHandle)

#define OS_Allocate(size)			UMI_Allocate(size)
#define OS_Free(pMemory)			UMI_Free(pMemory)
#define OS_MemoryCopy(pDest, pSrc, Size)	UMI_MemoryCopy(pDest, pSrc, Size)
#define OS_MemoryReset(pMemory, size)		UMI_MemoryReset(pMemory, size)
#define OS_MemoryEqual(pMem1, size1, pMem2, size2) UMI_MemoryCompare(pMem1, size1, pMem2, size2)
#define OS_GetTime()				UMI_GetTime()

#define OS_CREATE_LOCK()	UMI_AllocateLock()
#define OS_LOCK(x)		UMI_AcquireLock((LOCK_HANDLE)x)
#define OS_UNLOCK(x)		UMI_ReleaseLock((LOCK_HANDLE)x)
#define OS_DESTROY_LOCK(x)	UMI_DestroyLock((LOCK_HANDLE)x)

#define LOG_EVENT(_m, _id)	 if (SYS_DBG_MASK & (_m))UMI_DbgPrint(_id)
#define LOG_DATA(_m, _id, _d1)	 if (SYS_DBG_MASK & (_m))UMI_DbgPrint(_id, (_d1))
#define LOG_DATA2(_m, _id, _d1, _d2)      if (SYS_DBG_MASK & (_m))UMI_DbgPrint(_id, _d1, _d2)
#define LOG_DATA3(_m, _id, _d1, _d2, _d3) if (SYS_DBG_MASK & (_m))UMI_DbgPrint(_id, _d1, _d2, _d3)

#define OS_ASSERT(_exp_) {	\
	if (!(_exp_))		\
		UMI_Assert(__FILE__, __LINE__); \
}

#define CHECK_POINTER_IS_VALID(x)
#define CHECK_POINTER_FOR_ALIGNMENT(x)

#endif				/*_OS_IF_H */
