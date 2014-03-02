/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   xbusdma.c
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#include <misc/xbusdma.nmf>
#include <misc/xbusdma/include/xbusdma.h>
#include <dbc.h>

/**
 * Configuration function : 
 *
 * Configure Xbus Dma according periph and direction
 *
 */

t_uint16 get_xbusdma_interrupt(t_uint16 xdma_channel)
{
    switch (xdma_channel) {
        case 0: return 5;
        case 1: return 6;
        case 2: return 9;
        case 3: ITREMAP_REG7 = 32;
                return 7;
    }

    ASSERT(0); 
    return 0;
}


void enable_xbusdma_interrupt(t_uint16 xdma_channel)
{
    t_uint16 IT = get_xbusdma_interrupt(xdma_channel);

    SET_BIT(ITREMAP_interf_itmemol, IT);
    UNMASK_IT(IT);
}


void disable_xbusdma_interrupt(t_uint16 xdma_channel)
{
    t_uint16 IT = get_xbusdma_interrupt(xdma_channel);

    MASK_IT(IT);
    RESET_BIT(ITREMAP_interf_itmemol, IT);
}


void configure_xbusdma(AFM_XDMA_PERIPH periph, XbusDmaDirection direction,
                        t_uint16 xdma_channel, t_uint16 ssi_channel,
                        AFM_XDMA_ENDIANESS endianess)
{
    volatile XbusDmaConfig MMIO * DmaChannel = MAP_XBUSDMA_REGS(xdma_channel);
    t_uint16                      PeriphAddr = 0;
    t_uint16                      ExtLength  = 0;
    t_uint16                      IT;
    
    /* Precondition :
        - transmit can only be done on channels 1 or 3
        - receive  can only be done on channels 0 or 2
     */
    PRECONDITION((direction == xdma_receive &&
                  (xdma_channel == 0 || xdma_channel == 2))
                 ||
                 (direction == xdma_transmit &&
                  (xdma_channel == 1 || xdma_channel == 3)));
    /* - SSI is only connected to channel 2 and 3 */
    PRECONDITION(periph != AFM_XDMA_SSI || xdma_channel > 1);
    PRECONDITION(periph != AFM_XDMA_SSI || ssi_channel < 8);

    /* Stop ongoing transfer if any */
    RESET_BIT(DmaChannel->CTRL, start_bit);
    SET_BIT  (DmaChannel->CTRL, stop_bit);

    /* reset config */
    DmaChannel->CTRL = 0;

    if (endianess & AFM_XDMA_BYTE_ENDIANESS_CHANGE)
        SET_BIT(DmaChannel->CTRL, byte_swap_bit);

    if (endianess & AFM_XDMA_WORD_ENDIANESS_CHANGE)
        SET_BIT(DmaChannel->CTRL, word_swap_bit);

    if (direction == xdma_transmit)
        SET_BIT(DmaChannel->CTRL, direction_bit);
    
    SET_BIT(DmaChannel->CTRL, MMIO_type_32bit);
    SET_BIT(DmaChannel->CTRL, DSP_location_size_bit);
    SET_BIT(DmaChannel->CTRL, ext_request_bit);

    /* Configure Periph Address */
    switch(periph)
    {
        case AFM_XDMA_MSP0:
#ifdef __STN_8500
            PeriphAddr = 0xF92000U;  // TODO: 8500 MSP1
#else
            PeriphAddr = 0xF9B800U;  // TODO: Value only for 8820 TO BE UPDATED
#endif
            ExtLength  = 2;
            break;
        case AFM_XDMA_MSP1: 
#ifdef __STN_8500
            PeriphAddr = 0xF92000U;  // TODO: 8500 MSP1
#else
            PeriphAddr = 0xF9C000U;  // TODO: Value only for 8820 TO BE UPDATED
#endif
            ExtLength  = 2;
            break;
        case AFM_XDMA_MSP2:
#ifdef __STN_8500
            PeriphAddr = 0xF92000U;  // TODO: 8500 MSP1
#else
            PeriphAddr = 0xF9C800U;  // TODO: Value only for 8820 TO BE UPDATED
#endif
            ExtLength  = 2;
            break;
        case AFM_XDMA_SSI:
            if (direction == xdma_receive)
	    {
#ifdef HSIR_BASE
	      PeriphAddr = (t_uint16)(&HSIR_BUFFER0) + 2*ssi_channel;
#elif defined(SSIR_BASE)
	      PeriphAddr = (t_uint16)(&SSIR_rd0) + 2*ssi_channel;
#else
            ASSERT(0);
#endif
	    }
            else
	    {
#ifdef HSIT_BASE
	      PeriphAddr = (t_uint16)(&HSIT_BUFFER0) + 2*ssi_channel;
#elif defined(SSIT_BASE)
	      PeriphAddr = (t_uint16)(&SSIT_td0) + 2*ssi_channel;
#else
            ASSERT(0);
#endif
	    }
            ExtLength  = 2;
            break;
        default:
            ASSERT(0); 
            break;
    }

    DmaChannel->EXT_BASEH  = (PeriphAddr>>16) & 0xFFFF;
    DmaChannel->EXT_BASEL  = (PeriphAddr) & 0xFFFF;
    DmaChannel->EXT_LENGTH =  ExtLength;
    
    enable_xbusdma_interrupt(xdma_channel);
}


/**
 * Transfer function :
 *
 */
void start_xbusdma_transfer(t_uint16 xdma_channel, t_uint16 * buffer, t_uint16 buffer_size)
{
    volatile XbusDmaConfig MMIO * DmaChannel = MAP_XBUSDMA_REGS(xdma_channel);
    // TODO: add check buffer is in TCM
    
    enable_xbusdma_interrupt(xdma_channel);

    // configure transfer
    DmaChannel->INT_BASE   = (int)(buffer);
    DmaChannel->INT_LENGTH = buffer_size;
    
    // start transfer
    RESET_BIT(DmaChannel->CTRL, stop_bit);
    SET_BIT  (DmaChannel->CTRL, start_bit);
}

void start_xbusdma_transfer_to_memory(t_uint16 xdma_channel, t_uint16 * buffer, t_uint16 buffer_size, t_uint32 ext_addr)
{
    volatile XbusDmaConfig MMIO * DmaChannel = MAP_XBUSDMA_REGS(xdma_channel);
    
    enable_xbusdma_interrupt(xdma_channel);

    // configure transfer
    DmaChannel->INT_BASE   = (int)(buffer);
    DmaChannel->INT_LENGTH = 0;
    
    DmaChannel->EXT_BASEH  = ((ext_addr)>>16) & 0xFFFF;
    DmaChannel->EXT_BASEL  = (ext_addr) & 0xFFFF;
    DmaChannel->EXT_LENGTH =  buffer_size;

    RESET_BIT(DmaChannel->CTRL, ext_request_bit);

    // start transfer
    RESET_BIT(DmaChannel->CTRL, stop_bit);
    SET_BIT  (DmaChannel->CTRL, start_bit);
}


void stop_xbusdma_transfer(t_uint16 xdma_channel)
{
    volatile XbusDmaConfig MMIO * DmaChannel = MAP_XBUSDMA_REGS(xdma_channel);

    disable_xbusdma_interrupt(xdma_channel);

    // stop transfer
    RESET_BIT(DmaChannel->CTRL, start_bit);
    SET_BIT  (DmaChannel->CTRL, stop_bit);
}


void reset_xbusdma_channel(t_uint16 xdma_channel)
{
    volatile XbusDmaConfig MMIO * DmaChannel = MAP_XBUSDMA_REGS(xdma_channel);

    disable_xbusdma_interrupt(xdma_channel);

    DmaChannel->CTRL       = 0;
    DmaChannel->INT_BASE   = 0;
    DmaChannel->INT_LENGTH = 0;
    DmaChannel->EXT_BASEH  = 0;
    DmaChannel->EXT_BASEL  = 0;
    DmaChannel->COUNT      = 0;
    DmaChannel->EXT_LENGTH = 0;
}

