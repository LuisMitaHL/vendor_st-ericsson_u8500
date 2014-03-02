/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   dma_registers.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef  _DMA_REGISTERS_H_
#define  _DMA_REGISTERS_H_
#include "usefull.h"
   
#define DMA_REG_BASE_ADDR 0x801C0000UL
#define DMA_REG_END_ADDR  0x801C0FFFUL
//  with ext_io_base set on STM (0x80100000)
#define DMA_MMDSP_BASE_ADDR 0xFE0000UL

#define TOTAL_CHANNELS 32
#define TOTAL_LOGICAL_CHANNELS 128

#define REACH_CHANNEL(a)                (a-((a)%2))       // used to reach channel dedicated bits in registers which 
#define MASK_CHANNEL(a)                 (3UL<<REACH_CHANNEL(a))

#define LOS_TX  (1)
#define LOS_RX  (21)

typedef struct
{
    unsigned long SSCFG;
    unsigned long SSELT;
    unsigned long SSPTR;
    unsigned long SSLNK;
    unsigned long SDCFG;
    unsigned long SDELT;
    unsigned long SDPTR;
    unsigned long SDLNK;
} t_dma_std_channel_desc;

typedef struct {       // offset in byte (view from ARM core, divide by 2 for mmdsp)
    long GCC        ;  // 0x000  
    long PRTYP      ;
    long PRSM[2]    ;
    long PRMS[2]    ;
    long PRMO[2]    ;
    long LCPA       ;  // 0x020
    long LCLA       ;
    long SLCPA      ;
    long SLCLA      ;
    long SSEG[4]    ;
    long SCEG[4]    ;  // 0x040
    long ACTIV[2]   ; 
    long fake_0[2]  ;
    long PCMIS      ;  // 0x060
    long PCICR      ;
    long PCTIS      ;
    long PCEIS      ;
    long SPCMIS     ; 
    long SPCICR     ;
    long SPCTIS     ;
    long SPCEIS     ;
    long LCMIS[4]   ;  // 0x080
    long LCICR[4]   ;
    long LCTIS[4]   ;  // 0x0A0
    long LCEIS[4]   ;
    long SLCMIS[4]  ;  // 0x0C0
    long SLCICR[4]  ;
    long SLCTIS[4]  ;  // 0x0E0
    long SLCEIS[4]  ;
    long FSESS[2]   ;  // 0x100
    long FSEBS[2]   ;
    long PSEG[4]    ;
    long PCEG[4]    ;  // 0x120
    long RSEG[4]    ;
    long RCEG[4]    ;  // 0x140
    long fake_1[172];
    t_dma_std_channel_desc StdChannels[TOTAL_CHANNELS]; //0x800 is next free addr
    long fake_2[504];
    long PeriphID0  ;  
    long PeriphID1  ;  
    long PeriphID2  ;  
    long PeriphID3  ;  

} DMAConfig;

typedef  enum {
    DMA_BURST_4         = 0x1,
    DMA_BURST_8         = 0x2
} t_dma_burst_size;

#define AB8500_BURSTSIZE DMA_BURST_8

typedef enum {
    DMA_STOP            = 0,    /**> read: chl stopped;     write: stop chl if AS=DMA_SUSPENDED     */
    DMA_ACTIVE          = 1,    /**> read: chl running;     write: run if AS=DMA_STOP|DMA_SUSPENDED */
    DMA_SUSPEND_REQ     = 2,    /**> read: suspend request; write: susp req if AS=DMA_ACTIVE; flush if AS=DMA_SUSPENDED */
    DMA_SUSPENDED       = 3     /**> read: chl suspended;   write: no effect */
} t_dma_channel_activ_status;

typedef enum
{
    DMA_NO_MODE        = 0,    /**> read: ----- ;     write: no change     */
    DMA_BASIC_MODE     = 1,
    DMA_LOGICAL_MODE   = 2,
    DMA_OPERATION_MODE = 3 
} t_dma_channel_mode_status;

typedef enum
{
    DMA_SRCPHY_DSTLOG  = 1,
    DMA_SRCLOG_DSTPHY  = 2,
    DMA_SRCLOG_DSTLOG  = 3 
} t_dma_channel_logicalmode;

enum DmaLogicalChannel {
    DMA_LOG_SRC0,
    DMA_LOG_DST0,
    DMA_LOG_SRC1,
    DMA_LOG_DST1,
    DMA_LOG_SRC2,
    DMA_LOG_DST2,
    DMA_LOG_SRC3,
    DMA_LOG_DST3,
    DMA_LOG_SRC4,
    DMA_LOG_DST4,
    DMA_LOG_SRC5,
    DMA_LOG_DST5,
    DMA_LOG_SRC6,
    DMA_LOG_DST6,
    DMA_LOG_SRC7,
    DMA_LOG_DST7,
    DMA_LOG_SRC8,
    DMA_LOG_DST8,
    DMA_LOG_SRC9,
    DMA_LOG_DST9,
    DMA_LOG_SRC10,
    DMA_LOG_DST10,
    DMA_LOG_SRC11,
    DMA_LOG_DST11,
    DMA_LOG_SRC12,
    DMA_LOG_DST12,
    DMA_LOG_SRC13,
    DMA_LOG_DST13,
    DMA_LOG_SRC14,
    DMA_LOG_DST14,
    DMA_LOG_SRC15,
    DMA_LOG_DST15,
    DMA_LOG_SRC16,
    DMA_LOG_DST16,
    DMA_LOG_SRC17,
    DMA_LOG_DST17,
    DMA_LOG_SRC18,
    DMA_LOG_DST18,
    DMA_LOG_SRC19,
    DMA_LOG_DST19,
    DMA_LOG_SRC20,
    DMA_LOG_DST20,
    DMA_LOG_SRC21,
    DMA_LOG_DST21,
    DMA_LOG_SRC22,
    DMA_LOG_DST22,
    DMA_LOG_SRC23,
    DMA_LOG_DST23,
    DMA_LOG_SRC24,
    DMA_LOG_DST24,
    DMA_LOG_SRC25,
    DMA_LOG_DST25,
    DMA_LOG_SRC26,
    DMA_LOG_DST26,
    DMA_LOG_SRC27,
    DMA_LOG_DST27,
    DMA_LOG_SRC28,
    DMA_LOG_DST28,
    DMA_LOG_SRC29,
    DMA_LOG_DST29,
    DMA_LOG_SRC30,
    DMA_LOG_DST30,
    DMA_LOG_SRC31,
    DMA_LOG_DST31,
    DMA_LOG_SRC32,
    DMA_LOG_DST32,
    DMA_LOG_SRC33,
    DMA_LOG_DST33,
    DMA_LOG_SRC34,
    DMA_LOG_DST34,
    DMA_LOG_SRC35,
    DMA_LOG_DST35,
    DMA_LOG_SRC36,
    DMA_LOG_DST36,
    DMA_LOG_SRC37,
    DMA_LOG_DST37,
    DMA_LOG_SRC38,
    DMA_LOG_DST38,
    DMA_LOG_SRC39,
    DMA_LOG_DST39,
    DMA_LOG_SRC40,
    DMA_LOG_DST40,
    DMA_LOG_SRC41,
    DMA_LOG_DST41,
    DMA_LOG_SRC42,
    DMA_LOG_DST42,
    DMA_LOG_SRC43,
    DMA_LOG_DST43,
    DMA_LOG_SRC44,
    DMA_LOG_DST44,
    DMA_LOG_SRC45,
    DMA_LOG_DST45,
    DMA_LOG_SRC46,
    DMA_LOG_DST46,
    DMA_LOG_SRC47,
    DMA_LOG_DST47,
    DMA_LOG_SRC48,
    DMA_LOG_DST48,
    DMA_LOG_SRC49,
    DMA_LOG_DST49,
    DMA_LOG_SRC50,
    DMA_LOG_DST50,
    DMA_LOG_SRC51,
    DMA_LOG_DST51,
    DMA_LOG_SRC52,
    DMA_LOG_DST52,
    DMA_LOG_SRC53,
    DMA_LOG_DST53,
    DMA_LOG_SRC54,
    DMA_LOG_DST54,
    DMA_LOG_SRC55,
    DMA_LOG_DST55,
    DMA_LOG_SRC56,
    DMA_LOG_DST56,
    DMA_LOG_SRC57,
    DMA_LOG_DST57,
    DMA_LOG_SRC58,
    DMA_LOG_DST58,
    DMA_LOG_SRC59,
    DMA_LOG_DST59,
    DMA_LOG_SRC60,
    DMA_LOG_DST60,
    DMA_LOG_SRC61,
    DMA_LOG_DST61,
    DMA_LOG_SRC62,
    DMA_LOG_DST62,
    DMA_LOG_SRC63,
    DMA_LOG_DST63
};
typedef enum DmaLogicalChannel DmaLogicalChannel;

typedef  enum {
    DMA_MSP1_3_EE_OFFSET = 28,
    DMA_MSP0_EE_OFFSET   = 30
} t_dma_msp_ee;


enum DmaRealTime {
	DMA_NOT_REALTIME= 0, 
	DMA_REALTIME    = 1 
};
typedef enum DmaRealTime DmaRealTime;

enum DmaPriority {
	DMA_LOW_PRIORITY  = 0, 
	DMA_HIGH_PRIORITY = 1 
};
typedef enum DmaPriority DmaPriority;

enum DmaChannel {
	DMA_PHY0 = 0, 
	DMA_PHY1 = 1, 
	DMA_PHY2 = 2, 
	DMA_PHY3 = 3, 
	DMA_PHY4 = 4, 
	DMA_PHY5 = 5, 
	DMA_PHY6 = 6, 
	DMA_PHY7 = 7
};
typedef enum DmaChannel DmaChannel;

static volatile DMAConfig EXTMMIO * dmaReg = (volatile EXTMMIO DMAConfig *) DMA_MMDSP_BASE_ADDR;

static inline unsigned long getChannel(volatile long EXTMMIO dma_reg[], DmaChannel channel){
    return (dma_reg[(channel)%2] >> REACH_CHANNEL(channel)) & 0x3UL;
}

static inline void setChannel(volatile long EXTMMIO dma_reg[], DmaChannel channel, unsigned long value, unsigned int nochangevalue){
    long tmp = ((value)&0x3UL) << REACH_CHANNEL(channel);
    if(nochangevalue){
        tmp |= ~MASK_CHANNEL(channel);
    }
    dma_reg[(channel)%2] = tmp ;
}


#define WAIT_ENDOF_LOGICAL_TRANSFER(dma, regName, ee_offset)  while(((int)(dmaReg->StdChannels[dma].regName >> ee_offset)) & 0x3);
#define START_LOGICAL_TRANSFER(dma, regName, ee_offset)  while((((int)(dmaReg->StdChannels[dma].regName >> ee_offset)) & 0x1) == 0){ dmaReg->StdChannels[dma].regName |= (1UL << ee_offset); }
#define STOP_LOGICAL_TRANSFER(dma, regName, ee_offset)  if(((int)(dmaReg->StdChannels[dma].regName >> ee_offset)) & 0x1){ dmaReg->StdChannels[dma].regName = (dmaReg->StdChannels[dma].regName & ~(0x3UL<<ee_offset)) | (0x2UL<<ee_offset); while(((int)(dmaReg->StdChannels[dma].regName >> ee_offset)) & 0x3); }


static inline void resetMemoryParameter(volatile long EXTMMIO * param_buffer){
    param_buffer[0] = 0UL;
    param_buffer[1] = 0UL;
    param_buffer[2] = 0UL;
    param_buffer[3] = 0UL;
}

// following implementation is cryptic ... but optimised.
// DSP [47......0] maps HW register in : [xxxxxxxx][15-0][xxxxxxxx][31-16]  
static inline void writeParamInMemoryRx(volatile long EXTMMIO * param_buffer, unsigned int nb_element, unsigned long src_addr, unsigned long dst_addr, t_dma_burst_size burst_size){
    param_buffer[0] = (src_addr << 24) | ((long)(nb_element & 0xFFFF));
    param_buffer[1] = ((0x8201UL | (burst_size<<10)) << 24) | ((src_addr >> 16)  & 0xFFFFL);
    param_buffer[2] = (dst_addr << 24) | ((long)(nb_element & 0xFFFF));
    param_buffer[3] = ((0x1200UL | (burst_size<<10)) << 24) | ((dst_addr >> 16)  & 0xFFFFUL);
}

static inline void writeParamInMemoryTx(volatile long EXTMMIO * param_buffer, unsigned int nb_element, unsigned long src_addr, unsigned long dst_addr, t_dma_burst_size burst_size){
    param_buffer[0] = (src_addr << 24) | ((long)(nb_element & 0xFFFF));
    param_buffer[1] = ((0x1200UL | (burst_size<<10)) << 24) | ((src_addr >> 16)  & 0xFFFFUL);
    param_buffer[2] = (dst_addr << 24) | ((long)(nb_element & 0xFFFF));
    param_buffer[3] = ((0x8201UL | (burst_size<<10)) << 24) | ((dst_addr >> 16)  & 0xFFFFUL);
}

static inline void write_relink_params_tx(volatile long EXTMMIO * relink, unsigned int LOS, unsigned int nb_element, unsigned long src_addr, unsigned long dst_addr, t_dma_burst_size burst_size){
    relink[0] = (src_addr << 24) | ((long)(nb_element & 0xFFFF));
    relink[1] = ((0x1200UL | (LOS << 1) | (burst_size<<10)) << 24) | ((src_addr >> 16)  & 0xFFFFUL);
    relink[2] = (dst_addr << 24) | ((long)(nb_element & 0xFFFF));
    relink[3] = ((0x8201UL | ((LOS+1) << 1) | (burst_size<<10)) << 24) | ((dst_addr >> 16)  & 0xFFFFUL);
}

static inline void write_relink_params_rx (volatile long EXTMMIO * relink, unsigned int LOS, unsigned int nb_element, unsigned long src_addr, unsigned long dst_addr, t_dma_burst_size burst_size) {
    relink[0] = (src_addr << 24) | ((long)(nb_element & 0xFFFF));
    relink[1] = ((0x8201UL | (LOS << 1) | (burst_size<<10)) << 24) | ((src_addr >> 16)  & 0xFFFFUL);
    relink[2] = (dst_addr << 24) | ((long)(nb_element & 0xFFFF));
    relink[3] = ((0x1200UL | ((LOS+1) << 1) | (burst_size<<10)) << 24) | ((dst_addr >> 16)  & 0xFFFFUL);
}

static inline void write_first_link(volatile long EXTMMIO * param_buffer, volatile long EXTMMIO * link){
    param_buffer[0] = link[0];
    param_buffer[1] = link[1];
    param_buffer[2] = link[2];
    param_buffer[3] = link[3];
}

static inline void startRx(unsigned int nb_element, unsigned long arm_buffer, unsigned long msp_addr, unsigned int ee_offset, volatile long EXTMMIO * param_buffer, t_dma_burst_size burst_size){
    WAIT_ENDOF_LOGICAL_TRANSFER(DMA_PHY2, SSLNK, ee_offset);
    writeParamInMemoryRx(param_buffer, nb_element, msp_addr, arm_buffer, burst_size);
    START_LOGICAL_TRANSFER(DMA_PHY2, SSLNK, ee_offset);
}

static inline void startTx(unsigned int nb_element, unsigned long arm_buffer, unsigned long msp_addr, unsigned int ee_offset, volatile long EXTMMIO * param_buffer, t_dma_burst_size burst_size){
    WAIT_ENDOF_LOGICAL_TRANSFER(DMA_PHY2, SDLNK, ee_offset);
    writeParamInMemoryTx(param_buffer, nb_element, arm_buffer, msp_addr, burst_size);
    START_LOGICAL_TRANSFER(DMA_PHY2, SDLNK, ee_offset);
} 

static inline void startTxNoWait(unsigned int nb_element, unsigned long arm_buffer, unsigned long msp_addr, unsigned int ee_offset, volatile long EXTMMIO * param_buffer, t_dma_burst_size burst_size){
    writeParamInMemoryTx(param_buffer, nb_element, arm_buffer, msp_addr, burst_size);
    START_LOGICAL_TRANSFER(DMA_PHY2, SDLNK, ee_offset);
}

static inline void start_linked_tx(unsigned int nb_element, unsigned long arm_buffer, unsigned long msp_addr, unsigned int ee_offset, volatile long EXTMMIO * param_buffer, t_dma_burst_size burst_size){
    WAIT_ENDOF_LOGICAL_TRANSFER(DMA_PHY2, SDLNK, ee_offset);
    write_relink_params_tx(param_buffer, LOS_TX+2, nb_element, arm_buffer, msp_addr, burst_size);
    START_LOGICAL_TRANSFER(DMA_PHY2, SDLNK, ee_offset);
} 

static inline void start_linked_rx(unsigned int nb_element, unsigned long arm_buffer, unsigned long msp_addr, unsigned int ee_offset, volatile long EXTMMIO * param_buffer, t_dma_burst_size burst_size){
    WAIT_ENDOF_LOGICAL_TRANSFER(DMA_PHY2, SSLNK, ee_offset);
    write_relink_params_rx(param_buffer, LOS_RX+2, nb_element, msp_addr, arm_buffer, burst_size);
    START_LOGICAL_TRANSFER(DMA_PHY2, SSLNK, ee_offset);
}

static inline void stop_linked_tx( volatile long EXTMMIO * param_buffer){
    param_buffer[1] = ~((long)0xFE << 24) & param_buffer[1];
    param_buffer[3] = ~((long)0xFE << 24) & param_buffer[3];
}

static inline unsigned int get_next_linked_dma_job(volatile long EXTMMIO * param_buffer) {
    unsigned int LOS = (param_buffer[1] >> 25) & 0x7F;
    if (LOS == 0) {
        return 0xFF;
    } else {
        return LOS / 2;
    }
}

static inline void start_last_burst_tx(unsigned int nb_element, unsigned long arm_buffer, unsigned long msp_addr, unsigned int ee_offset, volatile long EXTMMIO * param_buffer, t_dma_burst_size burst_size){
    write_relink_params_tx(param_buffer, LOS_TX, nb_element, arm_buffer, msp_addr, burst_size);
    START_LOGICAL_TRANSFER(DMA_PHY2, SDLNK, ee_offset);
} 

#endif   // _DMA_REGISTERS_H_
