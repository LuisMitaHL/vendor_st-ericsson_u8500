/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#include <sleep/prcmu.nmf>
#include <inc/mmdsp_mapping.h>

int ATTR(myCoreId) = 1;

#define PRCMU_DSP_RETENTION_CMD         0 // JPF: whatever you want prcmu need != 2 && != 1 (default)

#define PRCMU_DSP_SVA_MAILBOX_IRQ       6 // JPF: PRCMU IT 18, bit 6
#define PRCMU_DSP_SIA_MAILBOX_IRQ       7 // JPF: PRCMU IT 19, bit 7

#define PRCMU_DSP_SIA_MM_IT_INDEX       0
#define PRCMU_DSP_SVA_MM_IT_INDEX       1

#define PRCMU_MBOX_CPU_SET          ((0x57100>>1) + MMIO_DSP_BASE_ADDR)
#define PRCMU_ARM_LS_CLAMP_SET      ((0x57420>>1) + MMIO_DSP_BASE_ADDR)
#define PRCMU_ARM_LS_CLAMP_CLR      ((0x57424>>1) + MMIO_DSP_BASE_ADDR)
#if defined(__STN_9540)
  #define PRCMU_DSP_SVA_MAILBOX_ADDR  ((0xBAFF4>>1) + MMIO_DSP_BASE_ADDR)
  #define PRCMU_DSP_SIA_MAILBOX_ADDR  ((0xBAFF6>>1) + MMIO_DSP_BASE_ADDR)
#elif (defined(__STN_8500) && (__STN_8500 >= 30))
    /* Value for first PRCMU on V2
     #define PRCMU_DSP_SVA_MAILBOX_ADDR  ((0xB8FEE>>1) + MMIO_DSP_BASE_ADDR) // JPF: Mb6 header (ReqMb6 not used)
     #define PRCMU_DSP_SIA_MAILBOX_ADDR  ((0xB8FEF>>1) + MMIO_DSP_BASE_ADDR) // JPF: Mb7 header (ReqMb7 not used)
     */
    // Value for new PRCMU V2 on V2
  #define PRCMU_DSP_SVA_MAILBOX_ADDR  ((0xB8FF4>>1) + MMIO_DSP_BASE_ADDR) // JPF: Mb6 header (ReqMb6 not used)
  #define PRCMU_DSP_SIA_MAILBOX_ADDR  ((0xB8FF6>>1) + MMIO_DSP_BASE_ADDR) // JPF: Mb7 header (ReqMb7 not used)
#elif defined(__STN_8500) && (__STN_8500 <= 20)
    #define PRCMU_DSP_SVA_MAILBOX_ADDR  ((0x5FFEE>>1) + MMIO_DSP_BASE_ADDR) // JPF: Mb6 header (ReqMb6 not used)
    #define PRCMU_DSP_SIA_MAILBOX_ADDR  ((0x5FFEF>>1) + MMIO_DSP_BASE_ADDR) // JPF: Mb7 header (ReqMb7 not used)
#else
    #error "Unsupported platform"
#endif

// TODO: Address must be change when PRCMU will separate SIA/SVA MBOX which is map only with same 16Bits address (and here same word)

static __EXTMMIO t_uint16 *pMailBox[] = {(__EXTMMIO t_uint16 *)PRCMU_DSP_SVA_MAILBOX_ADDR,
                                         (__EXTMMIO t_uint16 *)PRCMU_DSP_SIA_MAILBOX_ADDR};
static const unsigned int coreToIrqBitOffset[] = {PRCMU_DSP_SVA_MAILBOX_IRQ,
                                                  PRCMU_DSP_SIA_MAILBOX_IRQ};
static const unsigned int coreToMMItBitOffset[] = {PRCMU_DSP_SVA_MM_IT_INDEX,
                                                   PRCMU_DSP_SIA_MM_IT_INDEX};

void clearClampRegister(void ) {
    *((__EXTMMIO t_uint32 *) PRCMU_ARM_LS_CLAMP_CLR) = (1 << coreToMMItBitOffset[ATTR(myCoreId) - 2]);
}
/* implement sleep.api.prcmu.itf */
void METH(sendMsg)(t_prcmu_msg_type msg)
{
    switch(msg)
    {
        case PRCMU_MSG_GO_IN_RETENTION:
            // mask mmdsp interrupts so it allow to detect edge when wake up
            *((__EXTMMIO t_uint32 *) PRCMU_ARM_LS_CLAMP_SET) = (1 << coreToMMItBitOffset[ATTR(myCoreId) - 2]);
            // Inform prcmu to put mmdsp in retention
            *pMailBox[ATTR(myCoreId) - 2] = PRCMU_DSP_RETENTION_CMD;
            *((__EXTMMIO t_uint32 *) PRCMU_MBOX_CPU_SET) = (1 << coreToIrqBitOffset[ATTR(myCoreId) - 2]);   // JPF: Generate interrupt
            break;
        case PRCMU_MSG_END_OF_BOOT:
            // Demask mmdsp interrupts
            *((__EXTMMIO t_uint32 *) PRCMU_ARM_LS_CLAMP_CLR) = (1 << coreToMMItBitOffset[ATTR(myCoreId) - 2]);
            break;
        default:
            Panic(INTERNAL_PANIC, 0);
            break;
    }
}
