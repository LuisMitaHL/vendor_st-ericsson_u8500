/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/****************************************************************************
 * Remarks : Interrupt numbers are hard coded :
 *            - Interrupt #39 remapped 12 (DMA_IRQ_VCU_EOW) 
 *            - Interrupt #26 remapped 26 (MECC_IRQ)
 */
/****************************************************************************/
/* Include automatically built nmf file */
#include <mmdsp_api.h>
#include "resource_manager/interrupt.nmf"
#include "interrupt.h"
#include "dma_api.h"

/* Include global "shared w/ host" types and definitions */
#include <t1xhv_common.idt>

#include <trace/mpc_trace.h>
#pragma interface
Iresource_manager_api_signal cbrm2;
// #define call_interrupt(interrupt)							\
// 	if (interrupt_desc[interrupt].registered) 					\
// interrupt_desc[interrupt].cb->signalInterrupt(interrupt);
#define call_interrupt(interrupt)								\
	if (interrupt_desc[interrupt].registered) {					\
		cbrm2=*interrupt_desc[interrupt].cb;						\
		cbrm2.signalInterrupt(interrupt);							\
	}



/*------------------------------------------------------------------------
 * Global Variables
 *----------------------------------------------------------------------*/

ts_interrupt_desc interrupt_desc[NB_INTERRUPT];

/*------------------------------------------------------------------------
 * Private functions prototype
 *----------------------------------------------------------------------*/

/*------------------------------------------------------------------------
 * Methodes of component interface
 *----------------------------------------------------------------------*/

/*****************************************************************************/
/**
 * \brief  construct
 *
 * Init resource_manage /interrupt part. Implicitly called while instanciating 
 *    this component. 
 *    Initialize all internal variables (w/o any HW access).
 */
/*****************************************************************************/
#pragma force_dcumode
t_nmf_error  METH(construct)(void)
{
	t_uint16 i;

	//t1xhv_remap_interrupts();
	// IT 2, 11 (HW sem), 20, 31 (stack checker) not to be touched
	// ITREMAP_interf_reg20 = 42;      /*  20) VPP_SYNC\n          */

    /* unmask all Interrupts */
    MMDSP_EMU_MASKIT(INTERRUPT_ENABLE); 

    /* only unmask the interrupts needed for video ! 
       don't touch other interrupts otherwise we get problems 
       when running with other medias on SVA */
    ITREMAP_interf_itmskl  &= (0xFFFFU^(1U<<12));
    ITREMAP_interf_itmskh  &= (0xFFFFU^(1U<<(21-16))^(1U<<(26-16)));
#ifdef __ndk8500_a0__
    ITREMAP_interf_itmskh  &= (0xFFFFU^(1U<<(22-16)));
#endif

	// oldITREMAP_interf_reg12 = ITREMAP_interf_reg12;
	ITREMAP_interf_reg12 = 39;

	// oldITREMAP_interf_reg21 = ITREMAP_interf_reg21;
	ITREMAP_interf_reg21 = 41;

	// oldITREMAP_interf_reg26 = ITREMAP_interf_reg26;
	ITREMAP_interf_reg26 = 43;
	
#ifdef __ndk8500_a0__
	ITREMAP_interf_reg22 = 45; /* CABAC interrupt */
	ITREMAP_interf_reg30 = 62; /* ITC_CMP3 (for watchdog based on SVA timer) */
#endif

	for (i=0; i<NB_INTERRUPT; i++) {
		interrupt_desc[i].registered = FALSE;
	}
	 return NMF_OK;
} /* end of construct() function */

#pragma force_dcumode
void METH(destroy)(void)
{
  /* remask the interrupts that were unmasked for video */
  ITREMAP_interf_itmskl  |= (1U<<12);
  ITREMAP_interf_itmskh  |= (1U<<(21-16));
#ifdef __ndk8500_a0__
  ITREMAP_interf_itmskh  |= (1U<<(22-16));
#endif
	ITREMAP_interf_itmskh  |= (1U<<(26-16));
} /* end of destroy() function */
/*****************************************************************************/
/**
 * \brief	registerInterrupt
 * 
 * Register an interrupt line to a client.   
 *
 * \param	interrupt	interrupt number
 * \param	cb			call back to the client interface signal when interrupt raise
 */
/*****************************************************************************/
#pragma force_dcumode
void METH(subscribeInterrupt)(
		t_uint16 interrupt,
		Iresource_manager_api_signal *cb)
{
	printf("\n\nRM:Interrupt:: Entered into subscribeInterrupt = %d\n",interrupt);
	interrupt_desc[interrupt].registered = TRUE;
	interrupt_desc[interrupt].cb = cb;
} /* End of regsiterInterrupt() function. */

/*****************************************************************************/
/**
 * \brief	unRegisterInterrupt
 * 
 * unregister an already registered interrupt line. 
 *
 * \param	interrupt	interrupt number
 */
/*****************************************************************************/
#pragma force_dcumode
void METH(unsubscribeInterrupt)(t_uint16 interrupt)
{
	printf("\n\nRM:Interrupt:: Entered into unsubscribeInterrupt = %d\n",interrupt);
	interrupt_desc[interrupt].registered = FALSE;
} /* End of unRegisterInterrupt() function. */


/*****************************************************************************/
/**
 * \brief  Interrupt function for DMA_IRQ_VCU_EOW
 * \author SVA FW Team (source is extracted from old scheduler)
 *  
 *
 * Interrupt function for DMA_IRQ_VCU_EOW                                  	 
 */
/*****************************************************************************/
#pragma force_dcumode
EE_INTERRUPT void METH(IT12handler)(void)
{
#ifdef __ndk8500_a0__
	t_uint16 ret=(DMA_GET_REG(DMA_ISR_L)&CD_W) | (DMA_GET_REG(DMA_ISR_L)&CD_R);
	DMA_SET_REG_32(DMA_ISR_L,DMA_ISR_H,0xFFFFFFFFUL); /* ACKNOWLEDGE IT */
#elif defined __ndk8500_ed__
	t_uint16 ret=(DMA_GET_REG(DMA_ISR)&CD_W) | (DMA_GET_REG(DMA_ISR)&CD_R);
	DMA_SET_REG(DMA_ISR,0xFFFFU);  /* ACKNOWLEDGE IT */
#elif defined __ndk20__
	t_uint16 ret = 1;
	DMA_SET_REG(DMA_ISR,0x0);
#endif

	printf("\n\nRM:Interrupt:: IT12handler, ret = %d\n",ret);

	if(ret)
    {  
		call_interrupt(12); // BUFFER_EOW
    }
} /* End of IT12() function. */

/*****************************************************************************/
/**
 * \brief  Interrupt function for VPP_IRQ_EOT
 * \author SVA FW Team (source is extracted from old scheduler)
 *  
 * Interrupt function for VPP_IRQ_EOT                                  	 
 */
/*****************************************************************************/
#pragma force_dcumode
EE_INTERRUPT void METH(IT21handler)(void)
{
	printf("\n\nRM:Interrupt:: IT21handler");
	call_interrupt(21);  // VPP_EOT_IRQ
} /* End of IT21() function. */

/*****************************************************************************/
/**
 * \brief  Interrupt function for MECC_IRQ
 * \author SVA FW Team (source is extracted from old scheduler)
 *  
 * Interrupt function for MECC_IRQ
 */
/*****************************************************************************/
#pragma force_dcumode
EE_INTERRUPT void METH(IT26handler)(void)
{
	printf("\n\nRM:Interrupt:: IT26handler");
	call_interrupt(26); // MECC_IRQ
} /* End of IT26() function. */

/*****************************************************************************/
/**
 * \brief  Interrupt function for CABAC_IRQ
 * \author Maurizio Colombo
 *  
 * Interrupt function for CABAC_IRQ (H264 decode preprocessor)
 */
/*****************************************************************************/
#pragma force_dcumode
EE_INTERRUPT void METH(IT22handler)(void)
{
	printf("\n\nRM:Interrupt:: IT22handler");
	call_interrupt(22); // CABAC_IRQ
} /* End of IT22() function. */

/*****************************************************************************/
/**
 * \brief  Interrupt function for ITC_CMP3_IRQ
 * \author Maurizio Colombo
 *  
 * Interrupt function for ITC_CMP3_IRQ (watchdog based on SVA timer)
 */
/*****************************************************************************/
#pragma force_dcumode
EE_INTERRUPT void METH(IT30handler)(void)
{
	printf("\n\nRM:Interrupt:: IT30handler");
	call_interrupt(30); // ITC_CMP3_IRQ
} /* End of IT30() function. */



/*------------------------------------------------------------------------
 * Private functions methods
 *----------------------------------------------------------------------*/

/* End of interrupt.c file */
