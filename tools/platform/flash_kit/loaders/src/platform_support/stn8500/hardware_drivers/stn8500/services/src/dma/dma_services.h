/*
* --------------------------------------------------------------------------------------------
* STEricsson
* Reproduction and Communication of this document is strictly prohibited
* unless specifically authorized in writing by STEricsson
* --------------------------------------------------------------------------------------------
* File Name: dma_services.h
* This file contains the services function defintions.
* --------------------------------------------------------------------------------------------*/

#ifndef __SERVICES_DMA
#define __SERVICES_DMA

#include "dma.h"
#include "services.h"
#include "dma_irq.h"

#if defined(ST_8500V1) || defined(ST_HREFV1) || defined(__PEPS_8500_V1)|| defined(__PEPS_8500_V2)|| defined(ST_8500V2) || defined(ST_HREFV2)
#define SER_DMA_PRCMU_CLK_ENABLE 0x124
#define SER_DMA_PRCMU_CLK_OFFSET 0x74
#define SER_DMA_GCC_ENABLE_ALL   0xFF01
#endif


typedef void (*t_dma_callback_fct)(t_dma_chan_type,t_dma_line_num);



/******** Function Prototypes ********/
PUBLIC void SER_DMA_IntHandlerNotSecure(unsigned int);
PUBLIC void SER_DMA_IntHandlerSecure(unsigned int);
PUBLIC void SER_DMA_Init(t_uint8);
PUBLIC void SER_DMA_Close(void);
PUBLIC void SER_DMA_WaitEnd (IN t_dma_channel_config , IN t_dma_pipe_config);
PUBLIC void SER_DMA_WaitPoll (IN t_dma_channel_config , IN t_dma_pipe_config);
PUBLIC void  SER_DMA_RegisterCallback (t_dma_callback_fct fct);
#endif /*End of File */

