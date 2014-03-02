/*
* --------------------------------------------------------------------------------------------
* STEricsson
* Reproduction and Communication of this document is strictly prohibited
* unless specifically authorized in writing by STEricsson
* --------------------------------------------------------------------------------------------
* File Name: dma_services.c
* This module provides initialization environment for DMAC[Direct Memory Access Controller]
* -------------------------------------------------------------------------------------------- */

#include "hcl_defs.h"
#include "memory_mapping.h"

#include "dma.h"
#include "dma_irq.h"
#include "gic.h"

#include "services.h"
#include "dma_services.h"

#define DMA_PHY_CHAN_MAX          8
#define DMA_LOG_CHAN_MAX           128

/* Physical Relink Memory  (256K) + Logical Parameter Memory (2K) +  Logical Relink Memory (256K) = 514K*/
#define    RELINK_MEMORY_SIZE_MAX            514 * 1024
/*--------------------------------------------------------------------------*
 * Global Variables                                                         *
 *--------------------------------------------------------------------------*/
#pragma arm section zidata = "ahb_share_memory"
t_uint8 g_relink_memory[RELINK_MEMORY_SIZE_MAX]    /* 512 KB */ __attribute__((aligned (0x100000))); /* 1 MB aligned */
#pragma arm section zidata

PRIVATE volatile t_bool g_dma_phy_chan_flag_secure[DMA_PHY_CHAN_MAX];
PRIVATE volatile t_bool g_dma_phy_chan_flag_non_secure[DMA_PHY_CHAN_MAX];
PRIVATE volatile t_bool g_dma_log_chan_flag_secure[DMA_LOG_CHAN_MAX];
PRIVATE volatile t_bool g_dma_log_chan_flag_non_secure[DMA_LOG_CHAN_MAX];

PRIVATE t_dma_callback_fct	g_dma_callback_fct = 0;

#define NO_FILTER_MODE 0
/*--------------------------------------------------------------------------*
 * Private functions                                                        *
 *--------------------------------------------------------------------------*/
/****************************************************************************/
/* NAME:  SER_DMA_IntHandlerNotSecure                                       */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is Interrupt Handler for Non Secure DMA        */
/*                                                                          */
/* PARAMETERS:                                                              */
/*          unsigned int irq                                                */
/*                                                                          */
/* RETURN: void                                                             */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
void SER_DMA_IntHandlerNotSecure(unsigned int irq)
{
    t_dma_irq_src            irq_src;
    t_dma_chan_type         channel_type;
    t_dma_irq_status        irq_status;
    t_dma_event_desc        event_desc;
    t_gic_error             gic_error = GIC_OK;
    t_dma_line_num			line_num;

    /*    Check for the physical interrupt     */
    channel_type = DMA_CHAN_PHYSICAL_NOT_SECURE;
    irq_src = DMA_GetIRQSrcNonSecure(channel_type);
    if(DMA_NO_INTR == irq_src)
    {
        /*    Check for the logical interrupt     */
        channel_type = DMA_CHAN_LOGICAL_NOT_SECURE;
        irq_src = DMA_GetIRQSrcNonSecure(channel_type);
    }
    DMA_GetIRQSrcStatusNonSecure(channel_type, irq_src, &irq_status);
    DMA_ClearIRQSrc(channel_type , irq_src);

    gic_error = GIC_DisableItLine(GIC_DMA_LINE);
    if (gic_error != GIC_OK)
            {
                PRINT("GIC Binding error - %d\n", gic_error);
            }

    event_desc.channel_type =  channel_type;
    event_desc.irq_num  =  irq_src;
    line_num = (t_dma_line_num) event_desc.irq_num;

    // th_outputInfo(" IRQ = %d",event_desc.irq_src_num);

    DMA_FilterProcessIRQSrc(&irq_status, &event_desc, NO_FILTER_MODE);

    switch(event_desc.channel_type)
         {
             case DMA_CHAN_PHYSICAL_NOT_SECURE:
                  if (event_desc.irq_num > 7)
                     return;
                 g_dma_phy_chan_flag_non_secure[event_desc.irq_num] = irq_status.tc_intr;
                 if(((irq_status.tc_intr)) && (NULL!= g_dma_callback_fct))
          		  {
            		g_dma_callback_fct(channel_type, line_num);
            		 g_dma_phy_chan_flag_secure[event_desc.irq_num] = FALSE;
            	  }
             break;

             case DMA_CHAN_LOGICAL_NOT_SECURE:
                  if (event_desc.irq_num > 127)
                     return;
                 g_dma_log_chan_flag_non_secure[event_desc.irq_num] = irq_status.tc_intr;
                if(((irq_status.tc_intr)) && (NULL!= g_dma_callback_fct))
          		  {
          		  	g_dma_callback_fct(channel_type, line_num);
          		  	 g_dma_log_chan_flag_secure[event_desc.irq_num] = FALSE;
            	  }

             break;
         }

    gic_error = GIC_EnableItLine(GIC_DMA_LINE);
    if (gic_error != GIC_OK)
            {
                PRINT("GIC Binding error - %d\n", gic_error);
            }
}

/****************************************************************************/
/* NAME:  SER_DMA_IntHandlerSecure                                          */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is Interrupt Handler for Secure DMA            */
/*                                                                          */
/* PARAMETERS:                                                              */
/*          unsigned int irq                                                */
/*                                                                          */
/* RETURN: void                                                             */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
void SER_DMA_IntHandlerSecure(unsigned int irq)
{
    t_dma_irq_src           irq_src;
    t_dma_chan_type         channel_type = DMA_CHAN_PHYSICAL_SECURE;
    t_dma_irq_status        irq_status;
    t_dma_event_desc        event_desc;
    t_gic_error             gic_error = GIC_OK;
    t_dma_line_num          line_num;
    irq_src = DMA_GetIRQSrcSecure(channel_type);
    if(DMA_NO_INTR == irq_src)
    {
        channel_type = DMA_CHAN_LOGICAL_SECURE;
        irq_src = DMA_GetIRQSrcSecure(channel_type);
    }

    DMA_GetIRQSrcStatusSecure(channel_type, irq_src, &irq_status);
    DMA_ClearIRQSrc(channel_type , irq_src);

    gic_error = GIC_DisableItLine(GIC_DMA_SECURE_LINE);
    if (gic_error != GIC_OK)
            {
                PRINT("GIC Binding error - %d\n", gic_error);
            }

    event_desc.channel_type =  channel_type;
    event_desc.irq_num  =  irq_src;
	line_num = (t_dma_line_num) event_desc.irq_num;

    DMA_FilterProcessIRQSrc(&irq_status, &event_desc , NO_FILTER_MODE);

    switch(event_desc.channel_type)
    {
        case DMA_CHAN_PHYSICAL_SECURE:
            if (event_desc.irq_num > 7)
                     return;
            g_dma_phy_chan_flag_secure[event_desc.irq_num] = irq_status.tc_intr;
            if(((irq_status.tc_intr)) && (NULL!= g_dma_callback_fct))
            {
            	g_dma_callback_fct(channel_type, line_num);
            		 g_dma_phy_chan_flag_secure[event_desc.irq_num] = FALSE;
            }
        break;

        case DMA_CHAN_LOGICAL_SECURE:
            if (event_desc.irq_num > 127)
                     return;
            g_dma_log_chan_flag_secure[event_desc.irq_num] = irq_status.tc_intr;
            if(((irq_status.tc_intr)) && (NULL!= g_dma_callback_fct))
            {
            	g_dma_callback_fct(channel_type, line_num);
            	 g_dma_log_chan_flag_secure[event_desc.irq_num] = FALSE;

            }
        break;
    }

    gic_error = GIC_EnableItLine(GIC_DMA_SECURE_LINE);
    if (gic_error != GIC_OK)
            {
                PRINT("GIC Binding error - %d\n", gic_error);
            }
}

/****************************************************************************/
/* NAME:    SER_DMA_Init                                                    */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine initialize the dma environment                 */
/*                                                                          */
/* PARAMETERS: t_uint8                                                      */
/*                                                                          */
/* RETURN: void                                                             */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
PUBLIC void SER_DMA_Init(t_uint8 mask)
{
      t_system_address    relink_sys_addr;
      t_gic_func_ptr      old_datum;
      t_gic_error             gic_error = GIC_OK;

    if (DMA_OK != DMA_Init(DMA_CTRL_REG_BASE_ADDR))
    {
        PRINT("ERROR in DMA Initialisation");
        return;
    }

    DMA_SetBaseAddress(DMA_CTRL_REG_BASE_ADDR);

    relink_sys_addr.physical = ARM_TO_AHB_ADDR((t_physical_address)g_relink_memory);
    relink_sys_addr.logical = (t_logical_address)g_relink_memory;

    DMA_InitRelinkMemory(&relink_sys_addr , RELINK_MEMORY_SIZE_MAX);

#if defined(ST_8500V1) || defined(ST_HREFV1) || defined(__PEPS_8500_V1) || defined(__PEPS_8500_V2)|| defined(ST_8500V2) || defined(ST_HREFV2)

    (*(t_uint32 volatile *)DMA_CTRL_REG_BASE_ADDR)|= SER_DMA_GCC_ENABLE_ALL;
    // ER324895, do not reroute clock, rely on PRCMU to handle clock routing
    //(*(t_uint32 volatile *)(PRCMU_REG_BASE_ADDR + SER_DMA_PRCMU_CLK_OFFSET)) |= SER_DMA_PRCMU_CLK_ENABLE;
#endif

    /* Initialize interrupts */
    gic_error = GIC_ChangeDatum(GIC_DMA_LINE, (t_gic_func_ptr) SER_DMA_IntHandlerNotSecure, &old_datum);
    if (gic_error != GIC_OK)
            {
                PRINT("GIC Binding error - %d\n", gic_error);
            }
    gic_error = GIC_EnableItLine(GIC_DMA_LINE);
    if (gic_error != GIC_OK)
            {
                PRINT("GIC Binding error - %d\n", gic_error);
            }
    gic_error = GIC_ChangeDatum(GIC_DMA_SECURE_LINE, (t_gic_func_ptr) SER_DMA_IntHandlerSecure, &old_datum);
    if (gic_error != GIC_OK)
            {
                PRINT("GIC Binding error - %d\n", gic_error);
            }
    gic_error = GIC_EnableItLine(GIC_DMA_SECURE_LINE);
    if (gic_error != GIC_OK)
            {
                PRINT("GIC Binding error - %d\n", gic_error);
            }
}

/****************************************************************************/
/* NAME:  SER_DMA_WaitEnd                                                   */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is to stop waiting on DMA Interrupt            */
/* PARAMETERS:                                                              */
/*                                                                          */
/*                                                                          */
/* RETURN: void                                                             */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
PUBLIC void SER_DMA_WaitEnd (IN t_dma_channel_config chan_desc , IN t_dma_pipe_config pipe_desc)
{
    switch(chan_desc.mode)
    {
        case DMA_PHYSICAL:
            if(DMA_SECURE == chan_desc.security)
            {
                while (!g_dma_phy_chan_flag_secure[chan_desc.chan_num]);
                        g_dma_phy_chan_flag_secure[chan_desc.chan_num] = FALSE;
            }
            else
            {
                while (!g_dma_phy_chan_flag_non_secure[chan_desc.chan_num]);
                        g_dma_phy_chan_flag_non_secure[chan_desc.chan_num] = FALSE;
            }
        break;

        case DMA_LOGICAL:
            if(DMA_SECURE == chan_desc.security)
            {
                while (!g_dma_log_chan_flag_secure[pipe_desc.irq_num]);
                    g_dma_log_chan_flag_secure[pipe_desc.irq_num] = FALSE;
            }
            else
            {
                while (!g_dma_log_chan_flag_non_secure[pipe_desc.irq_num]);
                    g_dma_log_chan_flag_non_secure[pipe_desc.irq_num] = FALSE;
            }
      break;
    }
}

/****************************************************************************/
/* NAME:  SER_DMA_WaitPoll                                                  */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine is to poll on DMA                              */
/* PARAMETERS:                                                              */
/*                                                                          */
/*                                                                          */
/* RETURN: int                                                              */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
PUBLIC void SER_DMA_WaitPoll(IN t_dma_channel_config chan_desc , IN t_dma_pipe_config pipe_desc)
{
    t_dma_irq_src     irq_src   = DMA_IRQ_SRC_0; /* assign with any default value*/
    t_dma_chan_type     chan_type = DMA_CHAN_PHYSICAL_NOT_SECURE; /* assign with any default value*/

    if (DMA_PHYSICAL    ==    chan_desc.mode)
    {
        irq_src            =    (t_dma_irq_src)chan_desc.chan_num;

        if(DMA_NONSECURE    ==    chan_desc.security)
            chan_type        =    DMA_CHAN_PHYSICAL_NOT_SECURE;
        else
            chan_type        =    DMA_CHAN_PHYSICAL_SECURE;
    }
    else if (DMA_LOGICAL    ==    chan_desc.mode)
    {
        irq_src        =    (t_dma_irq_src)pipe_desc.irq_num;

        if(DMA_NONSECURE    ==    chan_desc.security)
            chan_type        =    DMA_CHAN_LOGICAL_NOT_SECURE;
        else
            chan_type        =    DMA_CHAN_LOGICAL_SECURE;
    }

    while(!(DMA_IsIRQSrcTC(chan_type , irq_src)));
    DMA_ClearIRQSrc(chan_type , irq_src);

}

/****************************************************************************/
/* NAME:    SER_DMA_Close                                                   */
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine stops and resets the dma environment           */
/*                                                                          */
/* PARAMETERS: void                                                         */
/*                                                                          */
/* RETURN: void                                                             */
/*                                                                          */
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
PUBLIC void SER_DMA_Close(void)
{
    //TBD
}

/****************************************************************************/
/* NAME:    SER_DMA_RegisterCallback 										*/
/*--------------------------------------------------------------------------*/
/* DESCRIPTION: This routine allows to connect a function that will be call */
/* on any DMA event.								                        */
/*                                                                          */
/* PARAMETERS:                                                              */
/*           - fct: function pointer                                        */
/*           															    */
/*                                                                          */
/* RETURN: void                                                             */
/*              		                                           			*/
/*--------------------------------------------------------------------------*/
/* REENTRANCY: NA                                                           */
/****************************************************************************/
PUBLIC void SER_DMA_RegisterCallback (t_dma_callback_fct fct)
{
	g_dma_callback_fct = fct;

}
