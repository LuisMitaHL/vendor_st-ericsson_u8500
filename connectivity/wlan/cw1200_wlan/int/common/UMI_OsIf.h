/*=============================================================================
 * Copyright (C) ST-Ericsson SA 2010
 * Author: wlan_humac.com
 * License terms: GNU General Public License (GPL), version 2.
 *===========================================================================*/
/**
 * \addtogroup Upper_MAC_Interface
 * \brief
 * This interface defines all the services UMAC implementation expects from
 * the operating system/platform.  This includes Timer services, Memory
 * Services, Debug API's, Endianness conversion macros etc.
 *
 */
/**
 * \file UMI_OsIf.h
 * - <b>PROJECT</b>		: WLAN HOST UMAC
 * - <b>FILE</b>		: UMI_OsIf.h
 * \brief
 * Header for OS Interface module. This file implements the interface as
 * described in WLAN_Host_UMAC_API.doc
 * \ingroup Upper_MAC_Interface
 * \date 05/01/09 14:42
 */

#ifndef _UMI_OS_IF_H
#define _UMI_OS_IF_H

#include <stddefs.h>

typedef void *LOCK_HANDLE;
typedef void *TIMER_HANDLE;
typedef void *UMI_WAIT_HANDLE;

typedef void (*TIMER_CB) (void *);

/******************************************************************************
 * MACROS
 ******************************************************************************
 * TO_STR(  s )   - Adds double quotes (") to s to form the string "s"
 * TO_CHAR( c )   - Adds single quotes (') to c to form the char   'c'
 * TO_TOK( b, v ) - Combines Base (b) with Value (v) to form single
 *				   token: BaseValue
 *
 * IN	     - Denotes a parameter as an INPUT
 * OUT	     - Denotes a parameter as an OUTPUT
 * RO	     - Denotes a member    as READ-ONLY
 * RW	     - Denotes a member    as READ-WRITE
 * WO	     - Denotes a member    as WRITE-ONLY
 * NA	     - Denotes a member    as NOT-AVAILABLE
 * PUBLIC    - Denotes a function  as PUBLIC
 * PRIVATE   - Denotes a function  as PRIVATE
 ******************************************************************************
 * Description:
 *
 * These macros are used only to aid the read-ability of the code.
 * The denoted constraints are not enforced at compile time.
 *****************************************************************************/
#ifndef IN
#define IN
#endif

#ifndef IN_OUT
#define IN_OUT
#endif

#ifndef OUT
#define OUT
#endif

#ifndef OPT_OUT
#define OPT_OUT
#endif

#ifndef OUT_BCOUNT_PART
#define OUT_BCOUNT_PART(size, length)
#endif

#ifndef OUT_ECOUNT
#define OUT_ECOUNT(size)
#endif

#ifndef RO
#define RO
#endif

#ifndef RW
#define RW
#endif

#ifndef WO
#define WO
#endif

#ifndef NA
#define NA
#endif

#ifndef PUBLIC
#define PUBLIC
#endif

#ifndef PRIVATE
#define PRIVATE static
#endif

#if !defined(FALSE)
/******************************************************************************
 * FALSE - Logical false
 *****************************************************************************/
#define FALSE	   0
#endif

#if !defined(TRUE)
/******************************************************************************
 * TRUE - Logical true is NOT( FALSE )
 *****************************************************************************/
#define TRUE	    1
#endif

#ifndef min
/******************************************************************************
 * min( A, B)  - Minimum of A and B
 *****************************************************************************/
#define    min(_a, _b)  (((_a) < (_b)) ? (_a) : (_b))
#endif

#ifndef max
/******************************************************************************
 * max( A, B)  - Maximum of A and B
 *****************************************************************************/
#define    max(_a, _b)  (((_a) > (_b)) ? (_a) : (_b))
#endif

/******************************************************************************
 * ASSERT
 *****************************************************************************/
#define UMI_OS_ASSERT(_exp)	{if (!(_exp)) UMI_Assert(__FILE__, __LINE__); }

/******************************************************************************
 * Endian Macros
 ******************************************************************************
 * Description:
 *
 * The following standard endian macros should be defined or indirectly
 * defined.
 *
 * cpu_to_le16()
 * le16_to_cpu()
 * cpu_to_le32()
 * le32_to_cpu()
 ******************************************************************************
 * Note:
 *
 * These are standard UNIX /Linux endian conversion macros.
 *****************************************************************************/

#ifndef le16_to_cpu
#define le16_to_cpu(_x) ((uint16)(_x))
#define le32_to_cpu(_x) ((uint32)(_x))
#define le64_to_cpu(_x) ((uint64)(_x))
#endif

#ifndef cpu_to_le16
#define cpu_to_le16(_x) ((uint16)(_x))
#define cpu_to_le32(_x) ((uint32)(_x))
#define cpu_to_le64(_x) ((uint64)(_x))
#endif

#ifndef be16_to_cpu
#define be16_to_cpu(x)			  \
	((((x) << 8) & 0xff00)		   \
      | (((x) >> 8) & 0x00ff)		 \
	)
#define be32_to_cpu(x)			  \
	((((x) << 24) & 0xff000000)	      \
      | (((x) <<  8) & 0x00ff0000)	    \
      | (((x) >>  8) & 0x0000ff00)	    \
      | (((x) >> 24) & 0x000000ff)	    \
	)
#define be64_to_cpu(_x)
#endif

#ifndef cpu_to_be16
#define cpu_to_be16(_x)
#define cpu_to_be32(_x)
#define cpu_to_be64(_x)
#endif

/*---------------------------------------------------------------------------*
 *	   FOLLOWING OS APIs TO BE DEFIEND BY HOST DRIVER	             *
 *---------------------------------------------------------------------------*/

/******************************************************************************
 * NAME:	UMI_Allocate
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This API is called by the UMAC when it needs memory for its different
 * operations
 * \param size    -   Size(in bytes) of the memory to be allocated.
 * \returns void* -   Pointer to the allocated memory.
 *****************************************************************************/
void *UMI_Allocate(uint32 size);

/******************************************************************************
 * NAME:	UMI_Free
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This API is called by the UMAC when the previously allocated memory needs to
 * be freed.
 * \param pMemory    -   Pointer to the memory which needs to be freed.
 *****************************************************************************/
void UMI_Free(void *pMemory);

/******************************************************************************
 * NAME:	UMI_MemoryReset
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This API is called by the UMAC when some memory buffer needs to be set to
 * zero.
 * \param pMemory -   Pointer to the memory which needs to be
 *                    reset(set to zeroo).
 * \param size    -   Size(in bytes) of the memory.
 *****************************************************************************/
void UMI_MemoryReset(void *pMemory, uint32 size);

/******************************************************************************
 * NAME:	UMI_MemoryCopy
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This API is called by the UMAC when it memory buffer needs to be copied from
 * one location to another.
 * \param pDest -   Pointer to the memory which needs to be reset.
 * \param pSrc  -   Pointer to the memory which needs to be reset.
 * \param Size  -   Size(in bytes) of the memory.
 *****************************************************************************/
void UMI_MemoryCopy(void *pDest, void *pSrc, uint32 Size);

/******************************************************************************
 * NAME:	UMI_CreateTimer
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This API creates a timer instance.
 * \param TimeOutCb       -   Pointer to UMAC timeout function.
 * \returns TIMER_HANDLE  -   Handle to timer instance.
 *****************************************************************************/
TIMER_HANDLE UMI_CreateTimer(TIMER_CB TimeOutCb);

/******************************************************************************
 * NAME:	UMI_StartTimer
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This API starts the timer instance.
 * \param timerHandle -   The Timer instance returned by UMI_CreateTimer().
 * \param mSec	-   Time in mili second.
 * \param data	-   Argument passed to the callback function.
 *****************************************************************************/
void UMI_StartTimer(TIMER_HANDLE timerHandle, uint32 mSec, void *data);

/******************************************************************************
 * NAME:	UMI_StopTimer
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This API creates stops the timer instance.
 * \param timerHandle  -   The Timer instance returned by UMI_CreateTimer().
 * \returns status     -   Success or Failure.
 *****************************************************************************/
uint8 UMI_StopTimer(TIMER_HANDLE timerHandle);

/******************************************************************************
 * NAME:	UMI_DestroyTimer
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This API destroys the timer instance.
 * \param timerHandle  -   The Timer instance returned by UMI_CreateTimer().
 *****************************************************************************/
void UMI_DestroyTimer(TIMER_HANDLE TimerHandle);

/******************************************************************************
 * NAME:	UMI_GetTime
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * Gets current system time.
 * \returns uint32  -   Current system time in mili second.
 *****************************************************************************/
uint32 UMI_GetTime(void);

/******************************************************************************
 * NAME:	UMI_DbgPrint
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * Output to the debugging console.
 * \param pFormat  -   The printf() style variable arguments.
 *****************************************************************************/
void UMI_DbgPrint(char *pFormat, ...);

/******************************************************************************
 * NAME:	UMI_AllocateLock
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * Creates handle for the lock.
 * \returns LOCK_HANDLE  -   Returns the handle to the locked section.
 *****************************************************************************/
LOCK_HANDLE UMI_AllocateLock(void);

/******************************************************************************
 * NAME:	UMI_AcquireLock
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * Obtains lock for critical section.
 * \param LOCK_HANDLE  -   Handle returned when UMI_AllocateLock was called for
 * this section.
 *****************************************************************************/
void UMI_AcquireLock(LOCK_HANDLE handle);

/******************************************************************************
 * NAME:	UMI_ReleaseLock
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * Releases the previously obtained lock.
 * \param LOCK_HANDLE  -   Handle returned when UMI_AllocateLock was called for
 *                         this section.
 *****************************************************************************/
void UMI_ReleaseLock(LOCK_HANDLE handle);

/******************************************************************************
 * NAME:	UMI_DestroyLock
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * Frees the lock allocated by UMI_AllocateLock() API.
 * \param LOCK_HANDLE  -   Handle returned when UMI_AllocateLock was called for
 *                         this section.
 *****************************************************************************/
void UMI_DestroyLock(LOCK_HANDLE handle);

/******************************************************************************
 * NAME:	UMI_Assert
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * ASSERT when some exception is occurred.
 * \param pFileName  -   Handle returned when UMI_AllocateLock was called for
 *                       this section.
 * \param lineNum    -   Handle returned when UMI_AllocateLock was called for
 *                       this section.
 *****************************************************************************/
void UMI_Assert(char *pFileName, uint32 lineNum);

/******************************************************************************
 * NAME:	UMI_MemoryCompare
 *---------------------------------------------------------------------------*/
/**
 * \brief
 * This function will compare two memory areas.
 * \param pMem1    -   Pointer to first memory area.
 * \param size1    -   Size of first memory area.
 * \param pMem2    -   Pointer to second memory area.
 * \param size2    -   Size of second memory area.
 * \returns uint32 -   TRUE if equal, FALSE otherwise.
 *****************************************************************************/
uint32 UMI_MemoryCompare(uint8 *pMem1,
			 uint32 size1,
			 uint8 *pMem2,
			 uint32 size2);

#endif	/* _UMI_OS_IF_H */
