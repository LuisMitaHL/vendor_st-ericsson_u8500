/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
 
#include <sleep/ulp.nmf>
#include <inc/archi-wrapper.h>
#include <inc/mmdsp_mapping.h>
#include <rtos/common/inc/misc.h>

#define TRACE_ENTRY         (0x746F8C)
#define TRACE_EXIT          (TRACE_ENTRY+1)

#define TRACE_ALLOW         (0xE5B200)
#define TRACE_PREVENT       (TRACE_ALLOW+0x100)

/* globals */
t_uint24 isSleeping = 0;
static t_sint24 sleepCounter = 0;
t_uint24 forceWakeup = 1;
static t_sleepNotifyCallback *pFirst;
static t_sleepNotifyCallback *pLast;

/* declare private methods */
void _init_othersp(int * val);
void _init_temporary_spU(void);
void setupCore(void);
void saveIndirect(t_uword addr, t_uint8 *pData, t_uword nbOfData);
void restoreIndirect(t_uword addr, t_uint8 *pData, t_uword nbOfData);
void saveYmemState(void);
void restoreYmemState(void);
void saveInstructionCacheState(void);
void restoreInstructionCacheState(void);
void saveDataCacheState(void);
void restoreDataCacheState(void);
void restoreInterruptState(void);
void saveRegistered(void);
void restoreRegistered(void);

/* implement ee.api.sleep.itf */
#pragma force_dcumode
void allowSleep()
{
    ENTER_CRITICAL_SECTION;
    sleepCounter--;
    traceUser(TRACE_ALLOW + (sleepCounter & 0xff));
    EXIT_CRITICAL_SECTION;
}

#pragma force_dcumode
void preventSleep()
{
    ENTER_CRITICAL_SECTION;
    sleepCounter++;
    traceUser(TRACE_PREVENT + (sleepCounter & 0xff));
    EXIT_CRITICAL_SECTION;
}

/* implement ee.api.sleepRegister.itf */
#pragma force_dcumode
void registerForSleep(t_sleepNotifyCallback *pCallback)
{
    ENTER_CRITICAL_SECTION;
    pCallback->pNext = 0;
    if (pFirst == 0) {
        pFirst = pCallback;
    } else {
        pLast->pNext = pCallback;
    }
    pLast = pCallback;
    
    EXIT_CRITICAL_SECTION;
}

#pragma force_dcumode
void unregisterForSleep(t_sleepNotifyCallback *pCallback)
{
    t_sleepNotifyCallback *pCurrent;
    t_sleepNotifyCallback *pPrev = 0;
    
    ENTER_CRITICAL_SECTION;
    pCurrent = pFirst;
    /* search elem */
    while(pCurrent) {
        if (pCurrent == pCallback) {break;}
        pPrev = pCurrent;
        pCurrent = pCurrent->pNext;
    }
    /* if found unlink it */
    if (pCurrent) {
        if (pPrev) {
            /* not the first elem */
            pPrev->pNext = pCurrent->pNext;
            if (pCurrent == pLast) {pLast = pPrev;}
        } else {
            /* first elem */
            pFirst = pCurrent->pNext;
            if (pCurrent == pLast) {pLast = pFirst;}
        }
    }
    
    EXIT_CRITICAL_SECTION;
}

//void NmfPrint0(int level, char* str);

/* implement sleep.api.ulp.itf */
void METH(enter)()
{
    ENTER_CRITICAL_SECTION;
    if (forceWakeup == 0 && sleepCounter < 0) {
       //NmfPrint0(0, "GO TO SLEEP......\n");

        traceUser(TRACE_ENTRY);
        saveRegistered();
        saveYmemState();
        saveInstructionCacheState();
        saveDataCacheState();
        isSleeping = 1;
        prcmu.sendMsg(PRCMU_MSG_GO_IN_RETENTION);
        // Loop forever prcmu stop the power
        while(1) ;
        //will never exit from here
    }
    EXIT_CRITICAL_SECTION;
}

/* implement code that will be call after going out of retention mode */
//TODO : to improve restart we can first restart I&D cache
// declare struct and data
#define ADDR_S_STACK_END &S_stack_end
#define ADDR_S_STACK_ &S_stack_

// define struct that save state
typedef struct {
    t_uint8 icache_mode_reg[1];
    t_uword dcache_mode_reg;
    t_uword dcache_control_reg;
    t_uword yMem_mode_reg;
} t_save_state;
static t_save_state coreState;

/* user C stack has been temporary initialize to system stack into crt0 code */
void sleep_wakeup()
{
    //setup core mode
    setupCore();
    //setup stacks
    if (((unsigned int)ADDR_S_STACK_END & 0x1) == 0) {
        _init_othersp((int *)(ADDR_S_STACK_END - 1));
        init_stackcheck_S(((unsigned int)ADDR_S_STACK_END -1 - (unsigned int)ADDR_S_STACK_), ADDR_S_STACK_);
    } else {
        _init_othersp((int *)ADDR_S_STACK_END);
        init_stackcheck_S(((unsigned int)ADDR_S_STACK_END - (unsigned int)ADDR_S_STACK_), ADDR_S_STACK_);
    }
    //clear_dcu_regs ?
    restoreYmemState();
    restoreInstructionCacheState();
    restoreDataCacheState();
    isSleeping = 0;
    sleepCounter = 0;
    restoreInterruptState();
    traceUser(TRACE_EXIT);
    restoreRegistered();
    prcmu.sendMsg(PRCMU_MSG_END_OF_BOOT);
    
    //NmfPrint0(0, "WAKE UP......\n");

    /* reenter scheduler */
    scheduler.enterAfterSleep();
}

asm void  _init_othersp(int * val) {
    set_other_sp @{val}
}

asm void setupCore()
{
    mode24
    xmvi #0x2000, adctl
}

/* save restore api */
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
void saveIndirect(t_uword addr, t_uint8 *pData, t_uword nbOfData)
{
    t_uword i;
    
    HOST[HOST_uaddrl] = addr;
    HOST[HOST_uaddrm] = 0;
    HOST[HOST_uaddrh] = 0;
    HOST[HOST_ucmd] = (0x10 | 0x04);
    for(i=0;i<nbOfData;i++)
    {
        *pData++ = HOST[HOST_udata0 + i];
    }
}
void restoreIndirect(t_uword addr, t_uint8 *pData, t_uword nbOfData)
{
    t_uword i;
    
    for(i=0;i<nbOfData;i++)
    {
        HOST[HOST_udata0 + i] =*pData++;
    }
    HOST[HOST_uaddrl] = addr;
    HOST[HOST_uaddrm] = 0;
    HOST[HOST_uaddrh] = 0;
    HOST[HOST_ucmd] = (0x10 | 0x00);
}

void saveYmemState()
{
    coreState.yMem_mode_reg = *(volatile __MMIO t_uword *)0xF608;
}

void restoreYmemState()
{
    *(volatile __MMIO t_uword *)0xF608 = coreState.yMem_mode_reg;
}

void saveInstructionCacheState()
{
    saveIndirect(0x02, coreState.icache_mode_reg, 1);
}

void restoreInstructionCacheState()
{
    restoreIndirect(0x02, coreState.icache_mode_reg, 1);
}

void saveDataCacheState()
{
    coreState.dcache_mode_reg = *(volatile __MMIO t_uword *)0xEC05;
    coreState.dcache_control_reg = *(volatile __MMIO t_uword *)0xEC06;
}

void restoreDataCacheState()
{
    *(volatile __MMIO t_uword *)0xEC06 = coreState.dcache_control_reg;
    *(volatile __MMIO t_uword *)0xEC05 = coreState.dcache_mode_reg;
}

void restoreInterruptState()
{
    ITREMAP_interf_itmskl   = 0xFFFF;
    ITREMAP_interf_itmskh   = 0xFFFF;
    ITREMAP_interf_itmemoh = 0xFFFFU;
    ITREMAP_interf_itmemol = 0xFFFFU;
}

#pragma noprefix
extern void *THIS;
void saveRegistered()
{
    t_sleepNotifyCallback *pCurrent = pFirst;
    void *oldThis;
    
    while(pCurrent) {
        oldThis = THIS;
        THIS = pCurrent->interfaceCallback.THIS;
        pCurrent->interfaceCallback.enter();
        THIS = oldThis;
        pCurrent = pCurrent->pNext;
    }
}

void restoreRegistered()
{
    t_sleepNotifyCallback *pCurrent = pFirst;
    void *oldThis;
    
    while(pCurrent) {
        oldThis = THIS;
        THIS = pCurrent->interfaceCallback.THIS;
        pCurrent->interfaceCallback.leave();
        THIS = oldThis;
        pCurrent = pCurrent->pNext;
    }
}

