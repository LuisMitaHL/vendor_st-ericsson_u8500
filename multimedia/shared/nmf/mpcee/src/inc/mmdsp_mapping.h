/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/* inc/mmdsp_mapping.h - EE.
 *
 * Copyright (c) 2006, 2007, 2008 STMicroelectronics.
 *
 * Reproduction and Communication of this document is strictly prohibited
 * unless specifically authorized in writing by STMicroelectronics.
 * 
 * Written by NMF team.
 */
#ifndef __INC_MMDSP_MAPPING_H
#define __INC_MMDSP_MAPPING_H

#include <inc/type.h>
#include <archi.h>

#ifdef __mode16__
#define DATA24_DSP_BASE_ADDR        0x10000UL
#define DATA16_DSP_BASE_ADDR        0x800000UL
#define MMIO_DSP_BASE_ADDR          0xF80000UL
#else /* __mode16__ -> __mode24__ */
#define DATA24_DSP_BASE_ADDR        0x10000U
#define DATA16_DSP_BASE_ADDR        0x800000U
#define MMIO_DSP_BASE_ADDR          0xF80000U
#endif /* __mode24__ */


#define EXT24_2_EXT16(addr)  (((((t_uint24)addr) - DATA24_DSP_BASE_ADDR)<<1) + DATA16_DSP_BASE_ADDR)
#define EXT16_2_EXT24(addr)  (((((t_uint24)addr) - DATA16_DSP_BASE_ADDR)>>1) + DATA24_DSP_BASE_ADDR)


#define MMDSP_ICACHE_LINE_SIZE_IN_INST 4

/*
 * Definition of some MMIOs
 * Theorically defined into periph.h of MMDSP Toolset but we exotic and fluctuent naming
 */
#define MMDSP_SEMAPHORE_BASE    0xF208U
#define MMDSP_IT_REMAP_BASE     0xFE00U

/* Definition of MMDSP interrupt sources */
/* Extracted from hamac_8815_cut20.pdf pp 24-26 */
#define AHB_M_INT_NUMBER        8

#define HOST_0_INT_NUMBER       11
#define HOST_1_INT_NUMBER       12
#define HOST_2_INT_NUMBER       13
#define HOST_3_INT_NUMBER       14

#define XBUS_DMA_INT_NUMBER     7
#define XBUS_DMA3_INT_NUMBER    32
#define XBUS_DMA2_INT_NUMBER    9
#define XBUS_DMA1_INT_NUMBER    6
#define XBUS_DMA0_INT_NUMBER    5

#define DCACHE_INT_NUMBER       15

#define TIMER0_INT_NUMBER       20
#define TIMER1_INT_NUMBER       19
#define TIMER2_INT_NUMBER       18

#define EXT_IT_0_INT_NUMBER       16
#define EXT_IT_1_INT_NUMBER       17
#define EXT_IT_2_INT_NUMBER       10
#define EXT_IT_3_INT_NUMBER       2
#define EXT_IT_4_INT_NUMBER       3
#define EXT_IT_5_INT_NUMBER       4
#define EXT_IT_6_INT_NUMBER       24
#define EXT_IT_7_INT_NUMBER       33
#define EXT_IT_8_INT_NUMBER       34
#define EXT_IT_9_INT_NUMBER       35
#define EXT_IT_10_INT_NUMBER      36
#define EXT_IT_11_INT_NUMBER      37
#define EXT_IT_12_INT_NUMBER      38

#define EXT_IT_TOG_INT_NUMBER    38

#define ARM_DMA0_INT_NUMBER     21
#define ARM_DMA1_INT_NUMBER     22
#define ARM_DMA2_INT_NUMBER     23
#define ARM_DMA3_INT_NUMBER     25
#define ARM_DMA4_INT_NUMBER     26
#define ARM_DMA5_INT_NUMBER     27
#define ARM_DMA6_INT_NUMBER     28
#define ARM_DMA7_INT_NUMBER     29
#define ARM_DMA_ALL_INT_NUMBER  30

#define MONITOR_INT_NUMBER      31

#define FAST_IT_0_INT_NUMBER      39
#define FAST_IT_1_INT_NUMBER      40
#define FAST_IT_2_INT_NUMBER      41
#define FAST_IT_3_INT_NUMBER      42
#define FAST_IT_4_INT_NUMBER      43
#define FAST_IT_5_INT_NUMBER      44
#define FAST_IT_6_INT_NUMBER      45
#define FAST_IT_7_INT_NUMBER      46
#define FAST_IT_8_INT_NUMBER      47
#define FAST_IT_9_INT_NUMBER      48
#define FAST_IT_10_INT_NUMBER      49
#define FAST_IT_11_INT_NUMBER      50
#define FAST_IT_12_INT_NUMBER      51
#define FAST_IT_13_INT_NUMBER      52
#define FAST_IT_14_INT_NUMBER      53

#define IRQ_CTL0_INT_NUMBER     54
#define IRQ_CTL1_INT_NUMBER     55
#define IRQ_CTL2_INT_NUMBER     56
#define IRQ_CTL3_INT_NUMBER     57
#define IRQ_CTL4_INT_NUMBER     58
#define IRQ_CMP0_INT_NUMBER     59
#define IRQ_CMP1_INT_NUMBER     60
#define IRQ_CMP2_INT_NUMBER     61
#define IRQ_CMP3_INT_NUMBER     62

/* Definition of HOST registers */
#define HOST_Inte0      0x07
#define HOST_Inte1      0x08
#define HOST_Int0       0x09
#define HOST_Int1       0x0a
#define HOST_Int_ris0   0x0b
#define HOST_Int_ris1   0x0c
#define HOST_Int_mis0   0x18
#define HOST_Int_mis1   0x1d
#define HOST_Int_icr0   0x11
#define HOST_Int_icr1   0x12
#define HOST_IntPol     0x0d
#define HOST_cmd0       0x13
#define HOST_cmd1       0x14
#define HOST_cmd2       0x15
#define HOST_cmd3       0x16

#define HOST_udata0     0x20
#define HOST_udata1     0x21
#define HOST_udata2     0x22
#define HOST_udata3     0x23
#define HOST_udata4     0x24
#define HOST_udata5     0x25
#define HOST_udata6     0x26
#define HOST_udata7     0x27
#define HOST_uaddrl     0x28
#define HOST_uaddrm     0x29
#define HOST_uaddrh     0x36
#define HOST_ucmd       0x2a

/*
 * Definition of indirect host registers
 */
#define IHOST_ICACHE_FLUSH_REG                            0x0
#define IHOST_ICACHE_FLUSH_CMD_ENABLE                     0x1
#define IHOST_ICACHE_FLUSH_ALL_ENTRIES_CMD                0x0U
#define IHOST_ICACHE_INVALID_ALL_UNLOCKED_L2_LINES_CMD    0x10U
#define IHOST_ICACHE_INVALID_ALL_LOCKED_L2_LINES_CMD      0x12U
#define IHOST_ICACHE_UNLOCK_ALL_LOCKED_L2_LINES_CMD       0x14U
#define IHOST_ICACHE_LOCK_ALL_WAYS_LESSER_THAN_LOCK_V_CMD 0x16U
#define IHOST_ICACHE_FLUSH_BY_SERVICE                     0x18U
#define IHOST_ICACHE_FLUSH_OUTSIDE_RANGE                  0x1AU

#define IHOST_ICACHE_END_CLEAR_REG                        0x19
#define IHOST_ICACHE_START_CLEAR_REG                      0x6

#define IHOST_ICACHE_LOCK_V_REG                           0x1
#define IHOST_ICACHE_FILL_STARTWAY_REG                    0x6
#define IHOST_ICACHE_MODE_REG                             0x2

/*
 * Definition of value of the ucmd register
 */
#define MMDSP_UCMD_WRITE                  0
#define MMDSP_UCMD_READ                   4
#define MMDSP_UCMD_CTRL_STATUS_ACCESS     0x10 
#define MMDSP_UCMD_DECREMENT_ADDR         0x20
#define MMDSP_UCMD_INCREMENT_ADDR         0x2


#define WRITE24_INDIRECT_HOST_REG(addr, value24) \
{ \
    HOST[HOST_uaddrl] = addr; \
    HOST[HOST_uaddrm] = 0;    \
    HOST[HOST_uaddrh] = 0;    \
    HOST[HOST_udata0] = ((value24 >> 0U) & 0xFFU);  \
    HOST[HOST_udata1] = ((value24 >> 8U) & 0xFFU);  \
    HOST[HOST_udata2] = ((value24 >> 16U) & 0xFFU);  \
    HOST[HOST_udata3] = 0;  \
    HOST[HOST_udata4] = 0;  \
    HOST[HOST_udata5] = 0;  \
    HOST[HOST_udata6] = 0;  \
    HOST[HOST_udata7] = 0;  \
    HOST[HOST_ucmd] = (MMDSP_UCMD_CTRL_STATUS_ACCESS | MMDSP_UCMD_WRITE); \
}

#define READ24_INDIRECT_HOST_REG(addr, value24) \
{ \
    HOST[HOST_udata0] = 0;  \
    HOST[HOST_udata1] = 0;  \
    HOST[HOST_udata2] = 0;  \
    HOST[HOST_udata3] = 0;  \
    HOST[HOST_udata4] = 0;  \
    HOST[HOST_udata5] = 0;  \
    HOST[HOST_udata6] = 0;  \
    HOST[HOST_udata7] = 0;  \
    HOST[HOST_uaddrl] = addr; \
    HOST[HOST_uaddrm] = 0;    \
    HOST[HOST_uaddrh] = 0;    \
    HOST[HOST_ucmd] = (MMDSP_UCMD_CTRL_STATUS_ACCESS | MMDSP_UCMD_READ); \
    value24 = (unsigned int)((((unsigned long int)HOST[HOST_udata0]) << 0ULL) |  \
        (((unsigned long int)HOST[HOST_udata1]) << 8ULL)  |  \
        (((unsigned long int)HOST[HOST_udata2]) << 16ULL));  \
}

#define MMDSP_FLUSH_ICACHE() \
{ /* Flush the Instruction cache */ \
    WRITE24_INDIRECT_HOST_REG(IHOST_ICACHE_FLUSH_REG, (IHOST_ICACHE_FLUSH_ALL_ENTRIES_CMD | IHOST_ICACHE_FLUSH_CMD_ENABLE)); \
}

#define MMDSP_FLUSH_ICACHE_UNLOCK_ONLY() \
{ /* Flush the unlock Instruction cache line*/ \
    WRITE24_INDIRECT_HOST_REG(IHOST_ICACHE_FLUSH_REG, (IHOST_ICACHE_INVALID_ALL_UNLOCKED_L2_LINES_CMD | IHOST_ICACHE_FLUSH_CMD_ENABLE)); \
}

#define MMDSP_FLUSH_ICACHE_BY_SERVICE(startAddr, endAddr) \
{ /* Flush the Instruction cache by service */ \
    unsigned int start_clear_addr = (startAddr) & ~(MMDSP_ICACHE_LINE_SIZE_IN_INST - 1); \
    unsigned int end_clear_addr = ((endAddr) + MMDSP_ICACHE_LINE_SIZE_IN_INST) & ~(MMDSP_ICACHE_LINE_SIZE_IN_INST - 1); \
    WRITE24_INDIRECT_HOST_REG(IHOST_ICACHE_START_CLEAR_REG, start_clear_addr); \
    WRITE24_INDIRECT_HOST_REG(IHOST_ICACHE_END_CLEAR_REG, end_clear_addr); \
    WRITE24_INDIRECT_HOST_REG(IHOST_ICACHE_FLUSH_REG, (IHOST_ICACHE_FLUSH_BY_SERVICE | IHOST_ICACHE_FLUSH_CMD_ENABLE)); \
}

// MMDSP_FLUSH_DCACHE() and MMDSP_FLUSH_DATABUFFER() moved to archi-wrapper.h

#if defined(__STN_8815)
#define MMDSP_FLUSH_DCACHE_BY_SERVICE(startAddr, endAddr)
#else /* __STN_8815 -> __STN_8820 or __STN_8500 */
#define MMDSP_FLUSH_DCACHE_BY_SERVICE(startAddr, endAddr) MMDSP_FLUSH_DCACHE()
#endif /* __STN_8820 or __STN_8500 */


#endif /* __INC_MMDSP_MAPPING_H */

