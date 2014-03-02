/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   msp_dma_controller.c
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/

// OST framework
#include "OstTraceDefinitions_ste.h"
#include "OpenSystemTrace_ste.h"
#ifdef OST_TRACE_COMPILER_IN_USE
  #include "audio_audiocodec_mpc_msp_dma_controller_src_msp_dma_controllerTraces.h"
#endif

#include <audiocodec/mpc/msp_dma_controller.nmf>
#include <dbc.h>
#include "prcc_registers.h"
#include "msp_registers.h"
#include "dma_registers.h"
#include "inc/archi-wrapper.h"

static long EXTMMIO * lcpa_buffer = NULL;
static long EXTMMIO * lcla_buffer = NULL;

static int mIsConfigured = 0;
static int mStartDMAControllerClientCount   = 0;

volatile MspConfig EXTMMIO * msp0_ptr = (volatile EXTMMIO MspConfig *) MSP0_MMDSP_BASE_ADDR;
volatile MspConfig EXTMMIO * msp1_ptr = (volatile EXTMMIO MspConfig *) MSP1_MMDSP_BASE_ADDR;
volatile MspConfig EXTMMIO * msp3_ptr = (volatile EXTMMIO MspConfig *) MSP3_MMDSP_BASE_ADDR;

static inline volatile MspConfig EXTMMIO * getMspRegisters(MspChannel channel){
    if(channel == MSP0){
        return msp0_ptr;
    }
    else if(channel == MSP1){
        return msp1_ptr;
    }
    return msp3_ptr;
}

static inline unsigned int getEventLine(MspChannel msp_channel){
    // MSP 1 and 3 shared same event line
    return msp_channel == MSP0 ? 15 : 14;
}

unsigned int AUDIO_MSP_GetInterruptLevel(MspChannel channel, MspDirection direction){
    if(channel == MSP0){
        if(direction == MSP_RX){
            return 26;
        }
        //MSP_TX
        return 28;
    }
    //MSP1 and MSP3 shares same IT line
    if(direction == MSP_RX){
        return 27;
    }
    //MSP_TX
    return 29;
}

DmaLogicalChannel getLogicalChannel(MspChannel msp_channel, MspDirection msp_direction){
    int tmp = 32 * (DMA_PHY2>>1);
    tmp += (getEventLine(msp_channel)) << 1;
    if(msp_direction == MSP_TX){
        tmp++;
    }
    return (DmaLogicalChannel)tmp;
}

void AUDIO_DMA_SetRealTime(DmaLogicalChannel logChannel, DmaRealTime real_time){
    unsigned int event_grp          = logChannel >> 5;
    unsigned int is_dst             = logChannel &  1;
    unsigned int event_line         = (logChannel >> 1) - (event_grp<<4);
    volatile long EXTMMIO * reg_ptr = real_time == DMA_REALTIME ? dmaReg->RSEG : dmaReg->RCEG;
    long value                      = is_dst ? (1UL << (event_line + 16)) : (1UL << event_line) ;
    reg_ptr[event_grp] = value;
}

void AUDIO_DMA_SetPriority(DmaLogicalChannel logChannel, DmaPriority priority){
    unsigned int event_grp          = logChannel >> 5;
    unsigned int is_dst             = logChannel &  1;
    unsigned int event_line         = (logChannel >> 1) - (event_grp<<4);
    volatile long EXTMMIO * reg_ptr = priority == DMA_HIGH_PRIORITY ? dmaReg->PSEG : dmaReg->PCEG;
    long value                      = is_dst ? (1UL << (event_line + 16)) : (1UL << event_line) ;
    reg_ptr[event_grp]              = value;
}

unsigned int getEEoffset(MspChannel msp_channel){
    return getEventLine(msp_channel) << 1;
}

void AUDIO_DMA_Start(DmaChannel channel){
    setChannel(dmaReg->ACTIV, channel, DMA_ACTIVE, 1);
}

void AUDIO_DMA_Stop(DmaChannel channel){
    if(getChannel(dmaReg->ACTIV, channel) == DMA_ACTIVE){
        setChannel(dmaReg->ACTIV, channel, DMA_SUSPEND_REQ, 1);
        while(getChannel(dmaReg->ACTIV, channel) != DMA_SUSPENDED);
        setChannel(dmaReg->ACTIV, channel, DMA_STOP, 1);
    }
}

void AUDIO_DMA_SetChannel_Physical(DmaChannel channel){
    AUDIO_DMA_Stop(channel);

    // Configure Phy Channel in Physical Mode
    setChannel(dmaReg->PRMS, channel, 1, 0);
    // Set both src and dst half-channel in logical mode
    setChannel(dmaReg->PRMO, channel, 1, 0);
}

//------------------------------------------------------
// Logical Channels
//------------------------------------------------------
void AUDIO_DMA_SetChannel_Logical(DmaChannel channel){
    long sscfg = 0;
    long sdcfg = 0;

    AUDIO_DMA_Stop(channel);

    // Configure Phy Channel in Logical Mode
    setChannel(dmaReg->PRMS, channel, DMA_LOGICAL_MODE, 0);
    // Set both src and dst half-channel in logical mode
    setChannel(dmaReg->PRMO, channel, DMA_SRCLOG_DSTLOG, 0);

    // Prepare register values 
    sscfg += 0x0 << 4; // Master port 0 used for parameter fetch
    sscfg += 0x1 << 5; // global it mask : it not masked
    sscfg += 0x0 << 6; // no endianness modif
    sscfg += 0x1 << 7; // high priority for the resource

    sdcfg += 0x0 << 4; // Master port 0 used for parameter fetch
    sdcfg += 0x1 << 5; // global it mask : it not masked
    sdcfg += 0x0 << 6; // no endianness modif
    sdcfg += 0x1 << 7; // high priority for the resource

    // Update registers
    dmaReg->StdChannels[channel].SSCFG = (dmaReg->StdChannels[channel].SSCFG & ~0xF0) | sscfg;
    dmaReg->StdChannels[channel].SSELT = channel << 8;
    dmaReg->StdChannels[channel].SDCFG = (dmaReg->StdChannels[channel].SDCFG & ~0xF0) | sdcfg;
    dmaReg->StdChannels[channel].SDELT = channel << 8;

    AUDIO_DMA_Start(channel);
}

void configureDMA(void){
    mIsConfigured = 1;

    startDMA();
}

static inline unsigned long getArmSideMSPAddr( MspChannel msp_channel ){
    if(msp_channel == MSP0){
        return MSP_0_REG_BASE_ADDR;
    }
    return MSP_1_REG_BASE_ADDR;
}

void start_first_linked_rx_xfer ( unsigned int nb_element, unsigned long arm_buffer, MspChannel msp_channel, t_dma_burst_size burst_size, unsigned int LOS) {
    unsigned int interruptLevel          = AUDIO_MSP_GetInterruptLevel(msp_channel, MSP_RX);
    volatile MspConfig EXTMMIO * mspReg  = getMspRegisters(msp_channel);
    unsigned int ee_offset               = getEEoffset(msp_channel);
    volatile long EXTMMIO * param_buffer = getLogicalParameterPtr(getLogicalChannel(msp_channel, MSP_RX));
    unsigned long msp_arm_addr           = getArmSideMSPAddr(msp_channel);

    write_relink_params_rx(param_buffer,LOS+2,nb_element, msp_arm_addr, arm_buffer, burst_size);
    ENTER_CRITICAL_SECTION;
    START_LOGICAL_TRANSFER(DMA_PHY2, SSLNK, ee_offset)

    mspReg->GCR   |= 0x1UL;
    EXIT_CRITICAL_SECTION;

    AUDIO_UNMASK_IT(interruptLevel);
}

void startFirstRxTransfer(unsigned int nb_element, unsigned long arm_buffer, MspChannel msp_channel, t_dma_burst_size burst_size){
    unsigned int interruptLevel          = AUDIO_MSP_GetInterruptLevel(msp_channel, MSP_RX);
    volatile MspConfig EXTMMIO * mspReg  = getMspRegisters(msp_channel);
    unsigned int ee_offset               = getEEoffset(msp_channel);
    volatile long EXTMMIO * param_buffer = getLogicalParameterPtr(getLogicalChannel(msp_channel, MSP_RX));
    unsigned long msp_arm_addr           = getArmSideMSPAddr(msp_channel);

    writeParamInMemoryRx(param_buffer, nb_element, msp_arm_addr, arm_buffer, burst_size);
    ENTER_CRITICAL_SECTION;
    START_LOGICAL_TRANSFER(DMA_PHY2, SSLNK, ee_offset);
    mspReg->GCR   |= 0x1UL;
    EXIT_CRITICAL_SECTION;

    AUDIO_UNMASK_IT(interruptLevel);
}

void startFirstTxTransfer(unsigned int nb_element, unsigned long arm_buffer, MspChannel msp_channel, t_dma_burst_size burst_size){
    unsigned int interruptLevel          = AUDIO_MSP_GetInterruptLevel(msp_channel, MSP_TX);
    volatile MspConfig EXTMMIO * mspReg  = getMspRegisters(msp_channel);
    unsigned int ee_offset               = getEEoffset(msp_channel);
    volatile long EXTMMIO * param_buffer = getLogicalParameterPtr(getLogicalChannel(msp_channel, MSP_TX));
    unsigned long msp_arm_addr           = getArmSideMSPAddr(msp_channel);

    ENTER_CRITICAL_SECTION;
    AUDIO_UNMASK_IT(interruptLevel);
    startTx(nb_element, arm_buffer, msp_arm_addr, ee_offset, param_buffer, burst_size);
    mspReg->GCR   |= 0x100;
    EXIT_CRITICAL_SECTION;
}

void startFirstTxLliTransfer(unsigned int nb_element, unsigned long arm_buffer, MspChannel msp_channel, t_dma_burst_size burst_size){
    unsigned int interruptLevel          = AUDIO_MSP_GetInterruptLevel(msp_channel, MSP_TX);
    volatile MspConfig EXTMMIO * mspReg  = getMspRegisters(msp_channel);
    unsigned int ee_offset               = getEEoffset(msp_channel);
    volatile long EXTMMIO * param_buffer = getLogicalParameterPtr(getLogicalChannel(msp_channel, MSP_TX));
    unsigned long msp_arm_addr           = getArmSideMSPAddr(msp_channel);

    AUDIO_UNMASK_IT(interruptLevel);
    start_linked_tx(nb_element, arm_buffer, msp_arm_addr, ee_offset, param_buffer, burst_size);
    mspReg->GCR   |= 0x100;
}

void stopTxTransfer(MspChannel msp_channel){
    unsigned int interruptLevel          = AUDIO_MSP_GetInterruptLevel(msp_channel, MSP_TX);
    volatile MspConfig EXTMMIO * mspReg  = getMspRegisters(msp_channel);
    unsigned int ee_offset               = getEEoffset(msp_channel);
    volatile long EXTMMIO * param_buffer = getLogicalParameterPtr(getLogicalChannel(msp_channel, MSP_TX));
    ENTER_CRITICAL_SECTION;

    // Mask IT before stop to ensure start not called during DMA stop
    AUDIO_MASK_IT(interruptLevel);

    STOP_LOGICAL_TRANSFER(DMA_PHY2, SDLNK, ee_offset);
    resetMemoryParameter(param_buffer);

    EXIT_CRITICAL_SECTION;
    // Waiting till fifo is empty
    while(!(mspReg->FLR & 0x10UL));

    // disabling Msp Tx
    mspReg->GCR &= ~0x0100UL;

	WAIT_ENDOF_LOGICAL_TRANSFER(DMA_PHY2, SDLNK, ee_offset);
}

void stopRxTransfer(MspChannel msp_channel){
    unsigned int interruptLevel          = AUDIO_MSP_GetInterruptLevel(msp_channel, MSP_RX);
    volatile MspConfig EXTMMIO * mspReg  = getMspRegisters(msp_channel);
    unsigned int ee_offset               = getEEoffset(msp_channel);
    volatile long EXTMMIO * param_buffer = getLogicalParameterPtr(getLogicalChannel(msp_channel, MSP_RX));

    volatile long dummy;
    ENTER_CRITICAL_SECTION;

    // Mask IT before stop to ensure start not called during DMA stop
    AUDIO_MASK_IT(interruptLevel);

    mspReg->GCR &= ~0x1UL;

    STOP_LOGICAL_TRANSFER(DMA_PHY2, SSLNK, ee_offset);

    EXIT_CRITICAL_SECTION;

    resetMemoryParameter(param_buffer);

    //flush Rx fifo. Warning: is it an issue that shift register is not cleared ????
    while(!(mspReg->FLR & 0x2UL)){
        dummy = mspReg->DR;
    }
}

void resetMSP(MspChannel msp_channel){
    volatile MspConfig EXTMMIO * mspReg  = getMspRegisters(msp_channel);

    mspReg->GCR   = 0x0UL;
    mspReg->IMSC  = 0x0UL;
    mspReg->TCF   = 0x0UL;
    mspReg->RCF   = 0x0UL;
    mspReg->DMACR = 0x0UL;
    mspReg->MCR   = 0x0UL;
    mspReg->IODLY = 0x0UL;

    switch ( msp_channel ){

        case MSP0:
            prcc1Reg->SOFTRST_CLR = 0x08;
            prcc1Reg->SOFTRST_SET = 0x08;
            break;

        case MSP1:
            prcc1Reg->SOFTRST_CLR = 0x10;
            prcc1Reg->SOFTRST_SET = 0x10;
            break;

        case MSP3:
            prcc1Reg->SOFTRST_CLR = 0x400;
            prcc1Reg->SOFTRST_SET = 0x400;
            break;

        default:
            ASSERT(0);
            break;
    }
}

void startDMA(void){
#ifdef DEBUG_TRACE
    OstTraceInt0(TRACE_DEBUG, "msp_dma_controller: startDMA ENTRY");
#endif
    if (!mIsConfigured) {
        return;
    }
    if(mStartDMAControllerClientCount++) {
        return;
    }
#ifdef DEBUG_TRACE
    OstTraceInt0(TRACE_DEBUG, "msp_dma_controller: startDMA do it");
#endif
    AUDIO_DMA_SetChannel_Logical(DMA_PHY2);
}

void stopDMA(void){
#ifdef DEBUG_TRACE
    OstTraceInt1(TRACE_DEBUG, "msp_dma_controller: stopDMA entry mStartDMAControllerClientCount:%d",mStartDMAControllerClientCount);
#endif
    mStartDMAControllerClientCount=0;
}

long EXTMMIO * getLogicalParameterPtr(DmaLogicalChannel logChannel){
    //OstTraceInt1(TRACE_DEBUG, "lcpa_buffer = %x", (int)lcpa_buffer);
    return lcpa_buffer + 4 * logChannel;
}

long EXTMMIO * get_relink_param_mem(DmaLogicalChannel logChannel, unsigned int LOS){

    unsigned long log_half_ch_offset = (dmaReg->StdChannels[DMA_PHY2].SDELT >> 8 ) & 0xFF;
    unsigned long phy_addr = dmaReg->LCLA + 1024 * log_half_ch_offset + 8 * LOS;

    //OstTraceInt1(TRACE_DEBUG, "get_relink_param_mem: log_half_ch_offset = %u", log_half_ch_offset);
    //OstTraceInt1(TRACE_DEBUG, "get_relink_param_mem: LOS = %u", LOS);
    //OstTraceInt2(TRACE_DEBUG, "get_relink_param_mem phy addr: %x %x", (phy_addr >> 16) & 0xFFFF, phy_addr & 0xFFFF);

    //return lcla_buffer + 256 * log_half_ch_offset + 2 * LOS;
    return (long EXTMMIO *) ((int)getMmdspAddr16(phy_addr));
}

void METH(start)(void) {
    lcpa_buffer = (long EXTMMIO *) ((int)getMmdspAddr16(dmaReg->LCPA));
    lcla_buffer = (long EXTMMIO *) ((int)getMmdspAddr16(dmaReg->LCLA));

    //OstTraceInt1(TRACE_DEBUG, "lcla_buffer = %x", (int)lcla_buffer);
    //OstTraceInt1(TRACE_DEBUG, "lcpa_buffer = %x", (int)lcpa_buffer);
    //----------------------------
    // Remap common Msp Interrupt
    // //TODO this is not common between MSP1 and msp0 so put it in right place !
    // Async_IT[0]  = mmdsp_IT[16] = MSP0 or MSP1
    ITREMAP_interf_reg30 = 16;
    // FIXME: fix mask un maks on use case transition
}

void METH(stop)(void) {
#ifdef DEBUG_TRACE
    OstTraceInt0(TRACE_DEBUG, "msp_dma_controller: stop ENTRY, calling stopDMA");
#endif
    stopDMA();
    AUDIO_MASK_IT(15);
    AUDIO_MASK_IT(30);
}

void METH(IT30handler)(void) {
#ifdef DEBUG_TRACE
    OstTraceInt0(TRACE_DEBUG, "msp_dma_controller: FIFO XRUN:\n");
#endif
}
