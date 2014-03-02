/*=============================================================================
 *
 *Linux Driver for CW1200 series
 *
 *
 *Copyright(c) ST-Ericsson SA, 2010
 *
 *This program is free software; you can redistribute it and/or modify it
 *under the terms of the GNU General Public License version 2 as published
 *by the Free Software Foundation.
 *
 *============================================================================= */
/**
 *addtogroup OS APIs used by UMAC
 *brief
 *
 */
/**
 *file : osif.c
 * -<b> PROJECT </b>			 : OS API
 * -<b> FILE </b>				: osif.c
 *brief
 *This file contains the Linux OS APIs used by the UMAC Module.
 *ingroup : OSIF
 *date 25/02/2010
 iiii*/


/*********************************************************************************
				INCLUDE FILES
 **********************************************************************************/

#include <linux/jiffies.h>
#include <linux/time.h>
#include <linux/timer.h>
#include <linux/spinlock.h>
#include <linux/wait.h>
#ifdef KERNEL_2_6_32
#include <linux/sched.h>
#endif
#include "cw1200_common.h"
#include "UMI_OsIf.h"
#include <linux/vmalloc.h>

#define USE_VMALLOC_SIZE 37000

/*********************************************************************************
 *			FUNCTION DEFINITIONS
 **********************************************************************************/
void *UMI_Allocate(uint32 size)
{
	if (in_interrupt())
		return kmalloc(size, GFP_ATOMIC);
	else {
		if (unlikely(size > USE_VMALLOC_SIZE))
			return vmalloc(size);

		return kmalloc(size, GFP_KERNEL);
	}
}


void  UMI_Free(void *pMemory)
{
	if (unlikely(is_vmalloc_addr(pMemory))) {
		DEBUG(DBG_MESSAGE, "Freeing vmalloc memory %p\n", pMemory);
		vfree(pMemory);
		pMemory = NULL;
	} else {
		kfree(pMemory);
		pMemory = NULL;
	}
}


void	UMI_MemoryReset(void *pMemory, uint32	size)
{
	memset(pMemory, 0, size);
}


void  UMI_MemoryCopy(void *pDest, void *pSrc, uint32 Size)
{
	memcpy(pDest, pSrc, Size);
}


TIMER_HANDLE	UMI_CreateTimer(TIMER_CB TimeOutCb)
{
	struct timer_list *umac_timer = NULL;

	umac_timer = kmalloc(sizeof(struct timer_list), GFP_ATOMIC);

	if (NULL == umac_timer) {
		DEBUG(DBG_ERROR, "Unable to allocate Timer\n");
	 return NULL;
	}

	init_timer(umac_timer);

	/*Type mismatch in the callback func expected */
	umac_timer->function = TimeOutCb;
	umac_timer->data = NULL;
	return (TIMER_HANDLE)umac_timer;

}



void  UMI_StartTimer(TIMER_HANDLE timerHandle,
		uint32 mSec, void *arg)
{
	struct timer_list *umac_timer = (struct timer_list *)timerHandle;
	uint32_t  expire = 0;

	expire = msecs_to_jiffies(mSec);
	umac_timer->data = (uint32_t) arg;
	mod_timer(umac_timer, jiffies + expire);
}

uint8	UMI_StopTimer(TIMER_HANDLE 	timerHandle)
{
	struct timer_list *umac_timer = (struct timer_list *)timerHandle;


	if (timer_pending(umac_timer))
		del_timer_sync(umac_timer);
	return 0;
}

void	UMI_DestroyTimer(TIMER_HANDLE 	TimerHandle)
{

	UMI_StopTimer(TimerHandle);
	kfree(TimerHandle);
}

uint32	UMI_GetTime()
{
	struct timeval tv;

	do_gettimeofday(&tv);
	return (tv.tv_sec*MSEC_PER_SEC) + (tv.tv_usec/MSEC_PER_SEC);
}


void UMI_DbgPrint(char *pFormat, ...)
{
	static char buf[1024] = {0};
	va_list ap;

	va_start(ap, pFormat);

	vsnprintf(buf, sizeof(buf)-1, pFormat, ap);
	printk(KERN_WARNING "%s", buf);
	va_end(ap);
}


LOCK_HANDLE  UMI_AllocateLock()
{
	spinlock_t *umac_slock = NULL;

	umac_slock = (spinlock_t *)kmalloc(sizeof(spinlock_t), GFP_ATOMIC)  ;

	if (NULL == umac_slock) {
		DEBUG(DBG_ERROR, "Unable to allocate spin lock\n");
	return NULL;
	}

	spin_lock_init(umac_slock);

	return (LOCK_HANDLE) umac_slock;
}

void  UMI_AcquireLock(LOCK_HANDLE handle)
{
	spinlock_t *umac_slock = (spinlock_t *)handle;

	spin_lock_bh(umac_slock);
}

void	UMI_ReleaseLock(LOCK_HANDLE 	handle)
{
	spinlock_t *umac_slock = (spinlock_t *)handle;

	spin_unlock_bh(umac_slock);
}


void	UMI_DestroyLock(LOCK_HANDLE 	handle)
{
	kfree(handle);
}

void UMI_Assert(char *pFileName, uint32 lineNum)
{
	atomic_xchg(&((sdio_cw_priv.driver_priv)->cw1200_unresponsive), TRUE);
	printk("%s, %u \n", pFileName, (uint32_t) lineNum);
	WARN_ON(1);
	/* Not the best place to have this */
	schedule_delayed_work(&sdio_cw_priv.unload_work, 0);
}



uint32 UMI_MemoryCompare(uint8 *pMem1,
		uint32 size1,
		uint8 *pMem2,
		uint32 size2
		)
{
	int32_t retval;
	if (size1 != size2)
		return FALSE;

	retval = memcmp(pMem1, pMem2, size1);

	if (retval == 0)
		return TRUE;
	else
		return FALSE;
}


UMI_WAIT_HANDLE  UMI_AllocateTimedWaitHandle(void)
{
	wait_queue_head_t *umac_wait = NULL;

	umac_wait = kmalloc(sizeof(wait_queue_head_t), GFP_ATOMIC);

	init_waitqueue_head(umac_wait);

	if (NULL == umac_wait) {
		DEBUG(DBG_ERROR, "Unable to allocate Wait Queue\n");
	return NULL;
	}

	return umac_wait;
}


uint32  UMI_TimedWait(UMI_WAIT_HANDLE WaitHandle, uint32 delay)
{
	wait_queue_head_t *umac_wait = (wait_queue_head_t *)WaitHandle;

	interruptible_sleep_on_timeout(umac_wait, msecs_to_jiffies(delay));
	return 1;
}


uint32  UMI_SignalWait(UMI_WAIT_HANDLE WaitHandle)
{
	wait_queue_head_t *umac_wait = (wait_queue_head_t *)WaitHandle;

	wake_up_interruptible(umac_wait);
	return 1;
}


uint32  UMI_ReleaseTimedWaitHandle(UMI_WAIT_HANDLE WaitHandle)
{
	kfree(WaitHandle);
	return 1;
}
