/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
/* rtos/common/src/panic.c - EE.
 *
 * Copyright (c) 2006, 2007, 2008 STMicroelectronics.
 *
 * Reproduction and Communication of this document is strictly prohibited
 * unless specifically authorized in writing by STMicroelectronics.
 *
 * Written by NMF team.
 */
/*
 *
 */
#include "rtos/common.nmf"

#include <rtos/common/inc/lowscheduler.h>
#include <rtos/perfmeter/inc/mpcload.h>
#include <inc/mmdsp_mapping.h>
#include <inc/archi-wrapper.h>
#include <share/communication/inc/nmf_service.h>

#if !defined(EXECUTIVE_ENGINE)
#include <stdio.h>
#include <stdlib.h>
#endif

#define MAGIC0 0xAAA
#define MAGIC1 0xBFA

/*
 * Attribute declaration
 */
volatile t_uint24 ATTR(serviceReason) = MPC_SERVICE_BOOT;
t_uint24 ATTR(serviceInfo0);
t_uint24 ATTR(serviceInfo1);
t_uint24 ATTR(serviceInfo2);

t_ee_debug_level ATTR(printLevel) = EE_DEBUG_LEVEL_WARNING;

//caution, the value of an attribute must be different from zero
//otherwise it will be in bss and automatically reset to zero at init
t_uint24 ATTR(panicDataAddr) = 1;

/*
 * PUBLIC void InitCommunication(void)
 */
t_nmf_error METH(construct)(void)
{
    /* Unmask usedIntLevel interrupt */
    ITREMAP_interf_itmskh &= ~(1U << (31 - 16)); // UnMask IT31

    /* Inform Host that DSP has booted */
    ATTR(serviceReason) = MPC_SERVICE_NONE;

    return NMF_OK;
}

/*
 * Panic
 */
static void service(t_uint24 reason, t_uint24 info0, t_uint24 info1, t_uint24 info2)
{
    ENTER_CRITICAL_SECTION;

    // Fill panic description
    ATTR(serviceReason) = reason;
    ATTR(serviceInfo0) = info0;
    ATTR(serviceInfo1) = info1;
    ATTR(serviceInfo2) = info2;

#if defined(EXECUTIVE_ENGINE)
    // Inform Host
    HOST[HOST_Inte1] = 0x1U; // Unmask DSP -> ARM interrupt
    HOST[HOST_Int1] = 1U;
#endif

    EXIT_CRITICAL_SECTION;

    // Loop until service description reset by host
    do { } while(ATTR(serviceReason) != MPC_SERVICE_NONE);
}

asm void * get_sp(void)
{
    xmv sp0, @{}
}

int tmp_ax1;
//int tmp_dst;
asm void save_registers(t_uint24* dst)
//asm void save_registers(void)
{
    //TODO, rl6 and axx3 are used by the compiler to perform the ... ops

    //save ax1 to tmp
    stx_f ax1, #_tmp_ax1
    //load ax1 to the panic area, where the reg file will be written
    xmv @{dst}, ax1

    stxpd_f Flag, ax1
    stxpd_f Adctl, ax1

    mode24

    //rh0, sp0, sp1, sp_other
    stxpd_f rh0, ax1
    xmv sp0, rh0
    stxpd_f rh0, ax1
    xmv sp1, rh0
    stxpd_f rh0, ax1
    get_other_sp rh0
    stxpd_f rh0, ax1

    //rl0, r1-r6
    stxpd_f rl0, ax1
    stxpd_f rh1, ax1
    stxpd_f rl1, ax1
    stxpd_f rh2, ax1
    stxpd_f rl2, ax1
    stxpd_f rh3, ax1
    stxpd_f rl3, ax1
    stxpd_f rh4, ax1
    stxpd_f rl4, ax1
    stxpd_f rh5, ax1
    stxpd_f rl5, ax1
    stxpd_f rh6, ax1
    stxpd_f rl6, ax1

    xsave2 ext01, rl6
    stxpd_f rl6, ax1

    xsave2 ext23, rl6
    stxpd_f rl6, ax1

    xsave2 ext45, rl6
    stxpd_f rl6, ax1

    xsave ext6, rl6
    stxpd_f rl6, ax1

    dmv ph, rl6
    stxpd_f rl6, ax1

    dmv pl, rl6
    stxpd_f rl6, ax1

    ldmmio_f    #_ITREMAP_interf_itmskl,rl6
    stxpd_f rl6, ax1

    ldmmio_f    #_ITREMAP_interf_itmskh,rl6
    stxpd_f rl6, ax1

    stxpd_f ix1, ax1
    stxpd_f ix2, ax1
    stxpd_f ix3, ax1
    stxpd_f ax2, ax1
    stxpd_f ax3, ax1
    //restore ax1 to ax2
    ldx_f #_tmp_ax1, ax2
    stxpd_f ax2, ax1

    stxpd_f axx1, ax1
    stxpd_f axx2, ax1
    stxpd_f axx3, ax1
    stxpd_f min1, ax1
    stxpd_f min2, ax1
    stxpd_f min3, ax1
    stxpd_f max1, ax1
    stxpd_f max2, ax1
    stxpd_f max3, ax1
}

//TODO, remove wrp, how does nosaveregs affect the code, ca be applied to panic()?
#pragma nosaveregs
void save_registers_wrp(t_uint24* dst)
{
    save_registers(dst);
}

static void panic(t_panic_reason reason, t_uint24 info1, t_uint24 info2)
{
    t_uint24 *pData = (t_uint24 *) ATTR(panicDataAddr);

    if (*pData != 1) {
        ENTER_CRITICAL_SECTION;
        //TODO, coredump, add magic
        pData[0] = MAGIC0;
        pData[1] = MAGIC1;
        pData[2] = (int)save_registers_wrp;
        save_registers_wrp(pData + 44);
        EXIT_CRITICAL_SECTION;
    }

    /* Trace Panic though XTI */
    nmfTracePanic(reason, *(t_uint24*)0x1, info1, info2);

#if defined(EXECUTIVE_ENGINE)
    // Since the panic will not been acknowledge by host, we will infinite loop in service
    service((t_uint24)reason,
            *(t_uint24*)0x1,
            info1,
            info2);
#else
    printf("PANIC(%d, %x, %x, %x)\n", ATTR(serviceReason), ATTR(serviceInfo0), ATTR(serviceInfo1), ATTR(serviceInfo2));
    exit(1);
#endif
}


#pragma force_dcumode
void Panic(t_panic_reason reason, t_uint24 info1) {
    MASK_ALL_ITS();
    panic(reason, info1, 0);
}

void NmfPrint0(t_ee_debug_level level, char* str) {
    if((int)ATTR(printLevel) >= (int)level)
    {
        service((t_uint24)MPC_SERVICE_PRINT,
                (t_uint24)str,
                0,
                0);

#if !defined(EXECUTIVE_ENGINE)
        printf(str);
#endif
    }
}

void NmfPrint1(t_ee_debug_level level, char* str, t_uword value1) {
    if((int)ATTR(printLevel) >= (int)level)
    {
        service((t_uint24)MPC_SERVICE_PRINT,
                (t_uint24)str,
                (t_uint24)value1,
                0);

#if !defined(EXECUTIVE_ENGINE)
        printf(str, value1);
#endif
    }
}

void NmfPrint2(t_ee_debug_level level, char* str, t_uword value1, t_uword value2) {
    if((int)ATTR(printLevel) >= (int)level)
    {
        service((t_uint24)MPC_SERVICE_PRINT,
                (t_uint24)str,
                (t_uint24)value1,
                (t_uint24)value2);

#if !defined(EXECUTIVE_ENGINE)
        printf(str, value1, value2);
#endif
    }
}

#define PRCMU_TIMER_4_DOWNCOUNT             ((0x57454>>1) + MMIO_DSP_BASE_ADDR)

t_uint24 ATTR(traceDataAddr) = 1;
t_uint24 ATTR(writePointer) = 1;
t_uint24 ATTR(lastWrittenTraceRevision) = 1;
t_uint24 ATTR(traceBufferSize) = 1;
t_uint24 ATTR(generateOstTraceIntr) = 1;
#define TRACE_THRESHOLD     (ATTR(traceBufferSize) / 2) // Send Dump every 1/2 buffer filled !!!

void NmfOstTrace(
        struct t_nmf_trace *trace,
        t_bool handleValid,
        t_uint8 paramNumber)
{
    int myWritePointer;
    int myTraceRevision;
    t_uint56 timestamp;
    __SHARED16 struct t_nmf_trace *traceDesc;

    ENTER_CRITICAL_SECTION;

    myWritePointer = ATTR(writePointer);
    myTraceRevision = ++ATTR(lastWrittenTraceRevision);

    ATTR(writePointer)++;
    if(ATTR(writePointer) >= ATTR(traceBufferSize))
        ATTR(writePointer) = 0;

    EXIT_CRITICAL_SECTION;

    traceDesc = &((__SHARED16 struct t_nmf_trace *)ATTR(traceDataAddr))[myWritePointer];
/*
    NmfPrint1(0, "ATTR(traceDataAddr) = %x\n", ATTR(traceDataAddr));
    NmfPrint1(EE_DEBUG_LEVEL_ERROR, "traceDesc = %x\n", traceDesc);
    NmfPrint1(0, "myWritePointer = %d\n", myWritePointer);
    NmfPrint1(0, "myTraceRevision = %d\n", myTraceRevision);
*/

    timestamp    = mpcload_GetCurrentTimerValue();
    traceDesc->timeStamp[0] = timestamp;
    traceDesc->timeStamp[1] = timestamp >> 16;
    traceDesc->timeStamp[2] = timestamp >> 32;
    traceDesc->timeStamp[3] = timestamp >> 48;

    traceDesc->componentId = trace->componentId;
    traceDesc->traceId = trace->traceId;
    traceDesc->paramOpt = trace->paramOpt;

    if(handleValid)
    {
        traceDesc->componentHandle = trace->componentHandle;
        traceDesc->parentHandle = trace->parentHandle;
    }
    else
    {
        traceDesc->componentHandle = 0;
        traceDesc->parentHandle = 0;
    }

    for(; paramNumber-- > 0; )
        traceDesc->params[paramNumber] = trace->params[paramNumber];

    traceDesc->revision = myTraceRevision;
#if defined(EXECUTIVE_ENGINE)
    if(((myTraceRevision % TRACE_THRESHOLD) == 0) && ATTR(generateOstTraceIntr))
    {
        ENTER_CRITICAL_SECTION;
        // NmfPrint1(EE_DEBUG_LEVEL_ERROR, "TRACE_THRESHOLD %d\n", myTraceRevision);
        ATTR(serviceReason) = MPC_SERVICE_TRACE;
        // Inform Host
        HOST[HOST_Inte1] = 0x1U; // Unmask DSP -> ARM interrupt
        HOST[HOST_Int1] = 1U;
        EXIT_CRITICAL_SECTION;
    }
#endif
}


/*
 * Stack exception management
 */
#define STACK_CHECK_STATUS (*(volatile __MMIO t_uword *)0xE801)
#define STACK_U_PC_VALUE_H (*(volatile __MMIO t_uword *)0xE810)
#define STACK_U_PC_VALUE_L (*(volatile __MMIO t_uword *)0xE811)
#define STACK_U_OVERFLOW_H (*(volatile __MMIO t_uword *)0xE80A)
#define STACK_U_OVERFLOW_L (*(volatile __MMIO t_uword *)0xE80B)
#define STACK_S_PC_VALUE_H (*(volatile __MMIO t_uword *)0xE818)
#define STACK_S_PC_VALUE_L (*(volatile __MMIO t_uword *)0xE819)
#define STACK_S_OVERFLOW_H (*(volatile __MMIO t_uword *)0xE812)
#define STACK_S_OVERFLOW_L (*(volatile __MMIO t_uword *)0xE813)
#define STACK_CHECK_STATUS_BIT0 (*(volatile __MMIO t_uword *)0x81A)
#define STACK_CHECK_STATUS_BIT1 (*(volatile __MMIO t_uword *)0x81B)
#define STACK_CHECK_STATUS_BIT2 (*(volatile __MMIO t_uword *)0x81c)

void callPanic(void* PC) {
    t_uint24 info2;

    if (STACK_CHECK_STATUS & 1)  {
		//bit[0] :  1  means overflow or underflow detected for U stack
	    info2 = (t_uint24)((STACK_U_OVERFLOW_H << 16) | STACK_U_OVERFLOW_L);
		PC = (void*)((STACK_U_PC_VALUE_H << 16) | STACK_U_PC_VALUE_L);
        STACK_CHECK_STATUS_BIT0 = 1;
        panic(USER_STACK_OVERFLOW, (t_uint24)PC, info2);
	} else if (STACK_CHECK_STATUS & 2)  {
		//bit[1] :  1  means overflow or underflow detected for S stack
	    info2 = (t_uint24)((STACK_S_OVERFLOW_H << 16) | STACK_S_OVERFLOW_L);
		PC = (void*)((STACK_S_PC_VALUE_H << 16) | STACK_S_PC_VALUE_L);
        STACK_CHECK_STATUS_BIT1 = 1;
        panic(SYSTEM_STACK_OVERFLOW, (t_uint24)PC, info2);
	} else if (STACK_CHECK_STATUS & 4)   {
		//bit[2] :  1  means unaligned long memory access
	    info2 = (t_uint24)get_other_sp0();
        STACK_CHECK_STATUS_BIT2 = 1;
        panic(UNALIGNED_LONG_ACCESS, (t_uint24)PC, info2);
	}
}

asm void* itHeader() {
	xmvi 	#0x2000, Adctl
	mode24
    pop		@{}
	push    @{}
}

#pragma nosaveregs
_INTERRUPT void IT31 (void) {
	callPanic(itHeader());
}

