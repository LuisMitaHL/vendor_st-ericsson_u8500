/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   xbusdma.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _XBUS_DMA_H_
#define _XBUS_DMA_H_

#include <misc/xbusdma/afmxdmaperiph.idt>
#include <archi.h>
#if 0 //def HSIR_BASE
#include <mmios_hsi.h>
#endif


typedef struct {
    int CTRL;
    int INT_BASE;
    int INT_LENGTH;
    int EXT_BASEH;
    int EXT_BASEL;
    int COUNT;
    int EXT_LENGTH;
} XbusDmaConfig;


/**
 * Explicit name for bit of CTRL register
 */
typedef enum {
    start_bit              = 0,
    direction_bit          = 1,
    stop_bit               = 2,
    byte_swap_bit          = 3,
    word_swap_bit          = 4,
    mem_counter_bit        = 5,
    physical_zone_bit      = 6,
    MMIO_type_32bit        = 7,
    DSP_location_size_bit  = 8,
    ext_request_bit        = 9,
    flow_controller_bit    = 10,
    priority_bit           = 11,
    gathered_interrupt_bit = 12,
    addr_mode_bit          = 13
} XbusDmaCtrlBit;


#define MAP_XBUSDMA_REGS(channel)   ((MMIO int *) &DMA0_CTRL + (channel) * 0x8)

#define   SET_BIT(a,b) ((a) |=  (1<<(b)))
#define RESET_BIT(a,b) ((a) &= ~(1<<(b)))

#define MASK_IT(it)     { SET_BIT(ITREMAP_interf_itmskl, (it)); \
                        ITREMAP_interf_itmskl; \
                        ITREMAP_interf_itmskl; \
                        ITREMAP_interf_itmskl; }

#define UNMASK_IT(it)   RESET_BIT(ITREMAP_interf_itmskl, (it))

#endif // _XBUS_DMA_H_
