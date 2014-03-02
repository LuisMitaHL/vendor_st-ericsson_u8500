/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */


/* Include automaticaly built nmf file */
#include <resource_manager/allocator.nmf>

#include "allocator.h"
#define EXTERN
#include "resource_manager_fifo.h"

#include "mmdsp_api.h"
#include "pxp_api.h"
#include "bdu_api.h"
#include "ckg_api.h"
#include "dma_api.h"
#include "stplug_api.h"

#define CLOCK_GATING_POWER_MGMT
#define HW_MUX3TO1_WAIT(a) (*(volatile __XIO t_uint16 *)(MUX3TO1_BASE+a))

/*------------------------------------------------------------------------
 * Global Variables
 *----------------------------------------------------------------------*/
EXTMEM t_t1xhv_debug_mode  rmDebugMode;		// debug mode : no HW accesses
EXTMEM t_uint8             hwInitDone;

/*------------------------------------------------------------------------
 * Private functions prototype
 *----------------------------------------------------------------------*/

static void informRessource(
		t_t1xhv_resource_status status,
		t_t1xhv_resource resource,
		Iresource_manager_api_inform_resource_status *cb);

static void hwInit(void);
static void hwDeInit(void);
extern void init_watchdog(void);
extern void deinit_watchdog(void);

/*------------------------------------------------------------------------
 * Methodes of component interface
 *----------------------------------------------------------------------*/

/*****************************************************************************/
/**
 * \brief  construct
 *
 * Init resource_manage / allocator part. Implicitly called while instanciating 
 *    this component. 
 *    Initialize all internal variables (w/o any HW access).
 */
/*****************************************************************************/
#pragma force_dcumode
t_nmf_error METH(construct)(void)
{
	t_uint16 i;

	/* Initialize internal private data */
	for (i=0; i<RESOURCE_MAX; i++) {
		rm_fifo_flush(i);
	}
  init_watchdog();
	rmDebugMode = DBG_MODE_NORMAL;
	hwInitDone  = FALSE;
	return NMF_OK;
} /* end of construct() function */

/*****************************************************************************/
/**
 * \brief  destroy
 *
 * DeInit resource_manage / allocator part. Implicitly called while instanciating 
 *    this component. 
 *    Initialize all internal variables (w/o any HW access).
 */
/*****************************************************************************/
#pragma force_dcumode
void METH(destroy)(void)
{
	/* Only take care to deInit the HW if it has been done previously. */
	if (hwInitDone == TRUE) hwDeInit();
	deinit_watchdog();
} /* end of destroy() function */

/*****************************************************************************/
/**
 * \brief  allocResource
 *
 * Allocate a specific resource. 
 * 
 * \param resource
 * \param cb
 */
/*****************************************************************************/
#pragma force_dcumode
void METH(allocResource)(
		t_t1xhv_resource resource, 
		Iresource_manager_api_inform_resource_status *cb)
{
	t_uint16 resource_available = FALSE;

	/* check if sleep was entered or not, in case sleep was entered reset hwInitDone back to FALSE */
	iIntSleep.statusInternalSleep();

	/* First of all, initialize hardware */
	if ((rmDebugMode == DBG_MODE_NORMAL) && (hwInitDone == FALSE)){
		/* Normal mode and first usage of this component : Do the HW Init. */
		hwInit();

		/* Memorize first HW init has been done. */
		hwInitDone  = TRUE;
	}

	if (resource >= RESOURCE_MAX){
		informRessource (STA_PARAM_ERROR, resource, cb);
		return;
	}

	if (is_rm_fifo_empty(resource)){
		// no running ressource
		resource_available = TRUE;
	}

	if (rm_fifo_push(resource, cb) != FALSE) {
		/* Ok, there's a place to store pending resource. */
		if (resource_available == TRUE){
			/* inform sleep component that a resource is being allocated to prevent going into sleep */
			iIntSleep.preventInternalSleep();
			/* Launch immediatly the required process */
			informRessource (STA_RESOURCE_FREE, resource, cb);
		}
	}
	else {
		/* No place, return an error. */
		informRessource (STA_ERROR_ALLOC_FULL, resource, cb); 
	}
} /* end of allocResource() function */

/*****************************************************************************/
/**
 * \brief  freeResource
 *
 * free a specific resource. 
 * 
 * \param s_resource
 */
/*****************************************************************************/
#pragma force_dcumode
void METH(freeResource)(t_t1xhv_resource resource, 
						Iresource_manager_api_inform_resource_status *cb)
{
//	t_uint16 is_running_available = FALSE;
	t_uint16 i;

	/* Input parameters test */
	if (resource >= RESOURCE_MAX) return;

	if (is_rm_fifo_empty(resource)) return;

	/* Consume the channel directly. */
	rm_fifo_remove(resource, cb);

	/* inform sleep component that a resource is being freed to re-allow going into sleep */
	iIntSleep.allowInternalSleep();

	if (! is_rm_fifo_empty(resource)){
		/* inform sleep component that a resource is being allocated to prevent going into sleep */
		iIntSleep.preventInternalSleep();
		informRessource (
				STA_RESOURCE_FREE,
				resource,
				get_rm_fifo_cb(resource));
	}

for (i=0; i<RESOURCE_MAX; i++) {
	 	if (! is_rm_fifo_empty(i)) return;
	 }
	  hwDeInit();
	// de initialization of HW if no running resource (to be enabled later for power management)
	// for (i=0; i<RESOURCE_MAX; i++) {
	// 	if (! is_rm_fifo_empty(i)) is_running_available = TRUE;
	// }
	// if (is_running_available = FALSE) hwDeInit();
} /* end of allocResource() function */

/*****************************************************************************/
/**
 * \brief  setDebug
 *
 * Set the debug mode of the resource_manager NMf component
 * 
 * \param  mode.
 * \param  param1.
 * \param  param2.
 */
/*****************************************************************************/
#pragma force_dcumode
void METH(setDebug)(t_t1xhv_debug_mode mode, t_uint32 param1, t_uint32 param2)
{
	if ((mode == DBG_MODE_NORMAL) || (mode == DBG_MODE_NO_HW)/* || (mode == DBG_MODE_PATTERN)*/){
		rmDebugMode = mode;
	}

	//oResource.setDebug(mode,param1,param2);
	//iIntSleep.setDebug(mode,param1,param2);
} /* End of setDebug() function. */


/*****************************************************************************/
/**
 * \brief  goneToSleep
 *
 * reset the flag hwInitDone (called by "sleep" subcomp only if sleep was entered)
 * this is called in response to iIntSleep.statusInternalSleep() call
 */
/*****************************************************************************/
#pragma force_dcumode
void METH(goneToSleep)()
{
	/* reset hw init done flag back to FALSE*/
	hwInitDone = FALSE;
}

/*****************************************************************************/
/**
 * \brief  mtfWrite
 *
 * Launch a memory copy from internal to internal|external area
 * 
 * \param  dstDataAddress.
 * \param  Size (in bytes).
 * \param  srcDataAddress.
 */
/*****************************************************************************/
#pragma force_dcumode
void METH(mtfWrite)(t_uint32 dstDataAddress, t_uint16 Size, t_uint32 srcDataAddress)
{
	if (	(rmDebugMode == DBG_MODE_NORMAL) && 
			(! is_rm_fifo_empty(RESOURCE_MTF))) {
		/* Check input parameters (masks) */
		if ((((dstDataAddress & ~1UL) & 0x3UL) != 0) || (Size & 0xFUL) != 0){
			informRessource (
					STA_PARAM_ERROR,
					RESOURCE_MTF,
					get_rm_fifo_cb(RESOURCE_MTF));
		}
		else{
			iMtf.writeThroughMTF(dstDataAddress, Size/2, (void *)(srcDataAddress));
		}
	}
} /* End of mtfWrite() function. */


/*****************************************************************************/
/**
 * \brief  mtfRead
 *
 * Launch a memory copy from internal|external to internal area
 * 
 * \param  srcDataAddress.
 * \param  Size (in bytes).
 * \param  dstDataAddress.
 */
/*****************************************************************************/
#pragma force_dcumode
void METH(mtfRead)(t_uint32 srcDataAddress, t_uint16 Size, t_uint32 dstDataAddress)
{
	if (	(rmDebugMode == DBG_MODE_NORMAL) && 
			(! is_rm_fifo_empty(RESOURCE_MTF))) {
		/* Check input parameters */
		if ((((srcDataAddress & ~1UL) & 0x3UL) != 0) || (Size & 0xFUL) != 0){
			informRessource (
					STA_PARAM_ERROR,
					RESOURCE_MTF,
					get_rm_fifo_cb(RESOURCE_MTF));
		}
		else{
			iMtf.readThroughMTF(srcDataAddress, Size/2, (void *)(dstDataAddress));
		}
	}
} /* End of mtfRead() function. */


/*------------------------------------------------------------------------
 * Internal Functions
 *----------------------------------------------------------------------*/

/*****************************************************************************/
/**
 * \brief  informRessource
 *
 * Call required interfaces. 
 * 
 * \param status	status
 * \param cb		call back
 */
/*****************************************************************************/
#pragma interface
Iresource_manager_api_inform_resource_status cb2;
static void informRessource(	
		t_t1xhv_resource_status status, 
		t_t1xhv_resource resource,
		Iresource_manager_api_inform_resource_status *cb)
{
	// call back of the interface delegation
	cb2=*cb;
	cb2.informResourceStatus(status, resource);	
} /* end of informRessource() function */


/*****************************************************************************/
/**
 * \brief  hwInit
 */
/*****************************************************************************/
static void hwInit(void)
{
	/* unmask all Interrupts */
	MMDSP_EMU_MASKIT(INTERRUPT_ENABLE);
	
	/* Enable clock for all HW blocks */
#ifndef CLOCK_GATING_POWER_MGMT
	CKG_ENABLE_CLOCK(ALL_BLOCKS,ALL_BLOCKS);
#else
	CKG_ENABLE_CLOCKS_RM();
#endif
	
	#ifdef __ndk8500_a0__
    DMA_SET_REG(DMA_BSM_L,(MECC_R|CD_W|CD_R|REC_W));
    DMA_SET_REG(DMA_BSM_H,(IME_CWL_R|IME_TPL_R|IME_SWL_R|IME_MVF_R|IME_MVF_W|IME_CCM_W|IPA_R|RECF_W|RECF_R));
	#else
	DMA_SET_REG(DMA_BSM,(MECC_R|CD_W|CD_R|REC_W));
	#endif
	
	/* CONFIGURE DMA priorities */
	#ifdef __ndk8500_a0__
    DMA_SET_REG_WAIT(DMA_PRIO_LEVEL0,(21<<3));    /* CUP_CTX_R   */
    DMA_SET_REG_WAIT(DMA_PRIO_LEVEL1,(13<<3));    /* MTF_R       */
    DMA_SET_REG_WAIT(DMA_PRIO_LEVEL2,(16<<3));    /* SWF_R       */
    DMA_SET_REG_WAIT(DMA_PRIO_LEVEL3,(14<<3));    /* CD_R        */
    DMA_SET_REG_WAIT(DMA_PRIO_LEVEL4,(4<<3));     /* VPP_L_R     */
    DMA_SET_REG_WAIT(DMA_PRIO_LEVEL5,(6<<3));     /* VPP_C_R     */
    DMA_SET_REG_WAIT(DMA_PRIO_LEVEL6,(17<<3));    /* VPP_PARAM_R */
    DMA_SET_REG_WAIT(DMA_PRIO_LEVEL7,(25<<3));    /* IMC_R       */
    DMA_SET_REG_WAIT(DMA_PRIO_LEVEL8,(24<<3));    /* IME_CWL_R   */
    DMA_SET_REG_WAIT(DMA_PRIO_LEVEL9,(10<<3));    /* IME_TPL_R   */
    DMA_SET_REG_WAIT(DMA_PRIO_LEVEL10,(9<<3));    /* IME_SWL_R   */
    DMA_SET_REG_WAIT(DMA_PRIO_LEVEL11,(23<<3));   /* IME_MVF_R   */
    DMA_SET_REG_WAIT(DMA_PRIO_LEVEL12,(8<<3));    /* IPA_R       */
    DMA_SET_REG_WAIT(DMA_PRIO_LEVEL13,(1<<3));    /* RECF_R      */
    DMA_SET_REG_WAIT(DMA_PRIO_LEVEL14,(11<<3));   /* IME_CCM_W   */
    DMA_SET_REG_WAIT(DMA_PRIO_LEVEL15,(12<<3));   /* MTF_W       */
    DMA_SET_REG_WAIT(DMA_PRIO_LEVEL16,(2<<3));    /* SWF_W       */
    DMA_SET_REG_WAIT(DMA_PRIO_LEVEL17,(19<<3));   /* CD_W        */
    DMA_SET_REG_WAIT(DMA_PRIO_LEVEL18,(3<<3));    /* VPP_L_W     */
    DMA_SET_REG_WAIT(DMA_PRIO_LEVEL19,(5<<3));    /* VPP_C_W     */
    DMA_SET_REG_WAIT(DMA_PRIO_LEVEL20,(7<<3));    /* REC_W       */
    DMA_SET_REG_WAIT(DMA_PRIO_LEVEL21,(20<<3));   /* CUP_CTX_W   */
    DMA_SET_REG_WAIT(DMA_PRIO_LEVEL22,(18<<3));   /* CUP_DBX_W   */
    DMA_SET_REG_WAIT(DMA_PRIO_LEVEL23,(22<<3));   /* IME_MVF_W   */
    DMA_SET_REG_WAIT(DMA_PRIO_LEVEL24,(15<<3));   /* RECF_W      */

    /* from HW overview document, recommended values */
    HW_MUX3TO1_WAIT(NOD_INIT1_PRIORITY) = 0x3;
    HW_MUX3TO1_WAIT(NOD_INIT2_PRIORITY) = 0x1;
    HW_MUX3TO1_WAIT(NOD_INIT3_PRIORITY) = 0x2;
	#endif

	#ifdef __ndk8500_ed__
	/* updated based on 8820 cut B STBus arbitration spec 1.0 - March 2008 */
	DMA_SET_REG_WAIT(DMA_PRIO_1,7<<3);   /* MTF_R   */
	DMA_SET_REG_WAIT(DMA_PRIO_2,10<<3);  /* SWF_R   */
	DMA_SET_REG_WAIT(DMA_PRIO_3,13<<3);  /* CD_R    */
	DMA_SET_REG_WAIT(DMA_PRIO_4,8<<3);   /* DEB_L_R */
	DMA_SET_REG_WAIT(DMA_PRIO_5,8<<3);   /* DEB_C_R */
	DMA_SET_REG_WAIT(DMA_PRIO_6,9<<3);   /* PARAM_R */
	DMA_SET_REG_WAIT(DMA_PRIO_7,11<<3);  /* MECC_R  */
	DMA_SET_REG_WAIT(DMA_PRIO_7A,11<<3);  /* CUP_CTX_R  */
	DMA_SET_REG_WAIT(DMA_PRIO_8,7<<3);   /* MTF_W   */
	DMA_SET_REG_WAIT(DMA_PRIO_9,10<<3);  /* SWF_W   */
	DMA_SET_REG_WAIT(DMA_PRIO_10,13<<3); /* CD_W    */
	DMA_SET_REG_WAIT(DMA_PRIO_11,8<<3);  /* DEB_L_W */
	DMA_SET_REG_WAIT(DMA_PRIO_12,8<<3);  /* DEB_C_W */
	DMA_SET_REG_WAIT(DMA_PRIO_13,12<<3); /* REC_W   */
	DMA_SET_REG_WAIT(DMA_PRIO_14,12<<3); /* CUP_CTX_W   */
	DMA_SET_REG_WAIT(DMA_PRIO_15,12<<3); /* CUP_DBX_W   */
	#endif

	#ifdef __ndk20__
	DMA_SET_REG_WAIT(DMA_PRIO_1,7);   /* MTF_R   */
	DMA_SET_REG_WAIT(DMA_PRIO_2,10);  /* SWF_R   */
	DMA_SET_REG_WAIT(DMA_PRIO_3,13);  /* CD_R    */
	DMA_SET_REG_WAIT(DMA_PRIO_4,8);   /* DEB_L_R */
	DMA_SET_REG_WAIT(DMA_PRIO_5,8);   /* DEB_C_R */
	DMA_SET_REG_WAIT(DMA_PRIO_6,9);   /* PARAM_R */
	DMA_SET_REG_WAIT(DMA_PRIO_7,14);  /* MECC_R  */
	DMA_SET_REG_WAIT(DMA_PRIO_8,7);   /* MTF_W   */
	DMA_SET_REG_WAIT(DMA_PRIO_9,10);  /* SWF_W   */
	DMA_SET_REG_WAIT(DMA_PRIO_10,13); /* CD_W    */
	DMA_SET_REG_WAIT(DMA_PRIO_11,8);  /* DEB_L_W */
	DMA_SET_REG_WAIT(DMA_PRIO_12,8);  /* DEB_C_W */
	DMA_SET_REG_WAIT(DMA_PRIO_13,12); /* REC_W   */
	#endif
	
	STPLUG_PGM();
} /* End of hwInit() function. */

/*****************************************************************************/
/**
 * \brief  hwDeInit
 */
/*****************************************************************************/
static void hwDeInit(void)
{
	/* Unregister all interrupt for any codec. */
	// iInterrupt.unRegisterInterrupt(ID_ALGO_ID_ANY);
	hwInitDone = FALSE;
	/* Disable all unused clock for all HW blocks */
	// CKG_ENABLE_CLOCK(CKG_MTF|CKG_VPP,CKG_DMA);
#ifdef CLOCK_GATING_POWER_MGMT
	CKG_DISABLE_CLOCKS_RM();
#endif
} /* End of hwDeInit() function. */
