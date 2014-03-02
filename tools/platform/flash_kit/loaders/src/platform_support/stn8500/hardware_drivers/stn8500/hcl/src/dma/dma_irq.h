/*
* --------------------------------------------------------------------------------------------
* STEricsson
* Reproduction and Communication of this document is strictly prohibited
* unless specifically authorized in writing by STEricsson
* --------------------------------------------------------------------------------------------
* File Name: dma_irq.h
* This file contains the interrupt request data structures and functions defintions
* --------------------------------------------------------------------------------------------*/

#ifndef __DMA_IRQ_HEADER
#define __DMA_IRQ_HEADER

#ifdef    __cplusplus
extern "C" 
{                  /* To allow C++ to use this header */
#endif

/* Includes */
#include "hcl_defs.h"

/* Enums */

/* This enum is used to define the 1interruot source of the dma controller
For the physical mode only 8 sources are valid, whereas for the logical mode 
all 128 sources are valid*/
typedef enum
{
    DMA_IRQ_SRC_0 = 0,
    DMA_IRQ_SRC_1,
    DMA_IRQ_SRC_2,
    DMA_IRQ_SRC_3,
    DMA_IRQ_SRC_4,
    DMA_IRQ_SRC_5,
    DMA_IRQ_SRC_6,
    DMA_IRQ_SRC_7,
    DMA_IRQ_SRC_8,
    DMA_IRQ_SRC_9,
    DMA_IRQ_SRC_10,
    DMA_IRQ_SRC_11,
    DMA_IRQ_SRC_12,
    DMA_IRQ_SRC_13,
    DMA_IRQ_SRC_14,
    DMA_IRQ_SRC_15,
    DMA_IRQ_SRC_16,
    DMA_IRQ_SRC_17,
    DMA_IRQ_SRC_18,
    DMA_IRQ_SRC_19,
    DMA_IRQ_SRC_20,
    DMA_IRQ_SRC_21,
    DMA_IRQ_SRC_22,
    DMA_IRQ_SRC_23,
    DMA_IRQ_SRC_24,
    DMA_IRQ_SRC_25,
    DMA_IRQ_SRC_26,
    DMA_IRQ_SRC_27,
    DMA_IRQ_SRC_28,
    DMA_IRQ_SRC_29,
    DMA_IRQ_SRC_30,
    DMA_IRQ_SRC_31,
    DMA_IRQ_SRC_32,
    DMA_IRQ_SRC_33,
    DMA_IRQ_SRC_34,
    DMA_IRQ_SRC_35,
    DMA_IRQ_SRC_36,
    DMA_IRQ_SRC_37,
    DMA_IRQ_SRC_38,
    DMA_IRQ_SRC_39,
    DMA_IRQ_SRC_40,
    DMA_IRQ_SRC_41,
    DMA_IRQ_SRC_42,
    DMA_IRQ_SRC_43,
    DMA_IRQ_SRC_44,
    DMA_IRQ_SRC_45,
    DMA_IRQ_SRC_46,
    DMA_IRQ_SRC_47,
    DMA_IRQ_SRC_48,
    DMA_IRQ_SRC_49,
    DMA_IRQ_SRC_50,
    DMA_IRQ_SRC_51,
    DMA_IRQ_SRC_52,
    DMA_IRQ_SRC_53,
    DMA_IRQ_SRC_54,
    DMA_IRQ_SRC_55,
    DMA_IRQ_SRC_56,
    DMA_IRQ_SRC_57,
    DMA_IRQ_SRC_58,
    DMA_IRQ_SRC_59,
    DMA_IRQ_SRC_60,
    DMA_IRQ_SRC_61,
    DMA_IRQ_SRC_62,
    DMA_IRQ_SRC_63,
    DMA_IRQ_SRC_64,
    DMA_IRQ_SRC_65,
    DMA_IRQ_SRC_66,
    DMA_IRQ_SRC_67,
    DMA_IRQ_SRC_68,
    DMA_IRQ_SRC_69,
    DMA_IRQ_SRC_70,
    DMA_IRQ_SRC_71,
    DMA_IRQ_SRC_72,
    DMA_IRQ_SRC_73,
    DMA_IRQ_SRC_74,
    DMA_IRQ_SRC_75,
    DMA_IRQ_SRC_76,
    DMA_IRQ_SRC_77,
    DMA_IRQ_SRC_78,
    DMA_IRQ_SRC_79,
    DMA_IRQ_SRC_80,
    DMA_IRQ_SRC_81,
    DMA_IRQ_SRC_82,
    DMA_IRQ_SRC_83,
    DMA_IRQ_SRC_84,
    DMA_IRQ_SRC_85,
    DMA_IRQ_SRC_86,
    DMA_IRQ_SRC_87,
    DMA_IRQ_SRC_88,
    DMA_IRQ_SRC_89,
    DMA_IRQ_SRC_90,
    DMA_IRQ_SRC_91,
    DMA_IRQ_SRC_92,
    DMA_IRQ_SRC_93,
    DMA_IRQ_SRC_94,
    DMA_IRQ_SRC_95,
    DMA_IRQ_SRC_96,
    DMA_IRQ_SRC_97,
    DMA_IRQ_SRC_98,
    DMA_IRQ_SRC_99,
    DMA_IRQ_SRC_100,
    DMA_IRQ_SRC_101,
    DMA_IRQ_SRC_102,
    DMA_IRQ_SRC_103,
    DMA_IRQ_SRC_104,
    DMA_IRQ_SRC_105,
    DMA_IRQ_SRC_106,
    DMA_IRQ_SRC_107,
    DMA_IRQ_SRC_108,
    DMA_IRQ_SRC_109,
    DMA_IRQ_SRC_110,
    DMA_IRQ_SRC_111,
    DMA_IRQ_SRC_112,
    DMA_IRQ_SRC_113,
    DMA_IRQ_SRC_114,
    DMA_IRQ_SRC_115,
    DMA_IRQ_SRC_116,
    DMA_IRQ_SRC_117,
    DMA_IRQ_SRC_118,
    DMA_IRQ_SRC_119,
    DMA_IRQ_SRC_120,
    DMA_IRQ_SRC_121,
    DMA_IRQ_SRC_122,
    DMA_IRQ_SRC_123,
    DMA_IRQ_SRC_124,
    DMA_IRQ_SRC_125,
    DMA_IRQ_SRC_126,
    DMA_IRQ_SRC_127,
	DMA_IRQ_SRC_128,
	DMA_NO_INTR = DMA_IRQ_SRC_128
}t_dma_irq_src;

/* ----- Interrupt Management M0 functions ------- */
/* These functions defines the interrupt management routines*/
PUBLIC void DMA_SetBaseAddress(IN t_logical_address dma_base_address);
PUBLIC void DMA_EnableIRQSrc(IN t_dma_chan_type ,IN t_dma_irq_src );
PUBLIC void DMA_DisableIRQSrc(IN t_dma_chan_type, IN t_dma_irq_src );
PUBLIC t_dma_irq_src DMA_GetIRQSrcNonSecure(IN t_dma_chan_type chan_type);
PUBLIC t_dma_irq_src DMA_GetIRQSrcSecure(IN t_dma_chan_type chan_type);
PUBLIC void DMA_GetIRQSrcStatusNonSecure(IN t_dma_chan_type , IN t_dma_irq_src , OUT t_dma_irq_status *);
PUBLIC void DMA_GetIRQSrcStatusSecure(IN t_dma_chan_type , IN t_dma_irq_src , OUT t_dma_irq_status *);
PUBLIC void DMA_ClearIRQSrc(IN t_dma_chan_type , IN t_dma_irq_src);
PUBLIC t_bool DMA_IsPendingIRQSrc(IN t_dma_chan_type chan_type,IN t_dma_irq_src dma_irq_src);
PUBLIC t_bool DMA_IsIRQSrcTC(IN t_dma_chan_type chan_type , IN t_dma_irq_src irq_src);

#ifdef __cplusplus
} /* To allow C++ to use this header */
#endif    /* __cplusplus */

#endif  /* __DMA_IRQ_HEADER */
