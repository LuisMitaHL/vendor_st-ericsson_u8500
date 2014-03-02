@
@ Copyright (C) ST-Ericsson SA 2011. All rights reserved.
@ This code is ST-Ericsson proprietary and confidential.
@ Any use of the code for whatever purpose is subject to
@ specific written permission of ST-Ericsson SA.
@
@------------------------------------------------------------------------------
@DESCRIPTION:
@------------------------------------------------------------------------------
@UPDATES
@ - Month day, year:
@ - Authors:
@ - Reason: L1 cache/MMU drivers
@------------------------------------------------------------------------------
@

    @PRESERVE8
    .balign 8

    .global XL_EnableMmu
    .global XL_EnableIcache
    .global XL_EnableDcache
    .global XL_DisableMmu
    .global XL_DisableIcache
    .global XL_DisableDcache
    .global XL_InvalidateIcache
    .global XL_InvalidateDcache
    .global XL_CleanDcache
    .global XL_CleanInvDcache
    .global XL_ReadCp15CtrlReg
    .global XL_ReadCpsr
    .global XL_SetStackPointer
    .global XL_SetCpu1MmuCache
    .global XL_GetMMUBaseAddress
    .global XL_SetMMUBaseAddress
    .global XL_InvalidateIcacheArea
    .global XL_InvalidateDcache
    .global XL_InvalidateDcacheArea
    .global XL_CleanDcache
    .global XL_CleanDcacheArea
    .global BOOT_TTBR0_ADDR

@
@ CP15 cache cleaning constant definition
@
@
    .set LINE_FIELD_OFFSET,     5       @ start of line number field offset in
                                        @ way/index format
    .set LINE_FIELD_OVERFLOW,   13      @ Warning: this assumes a 256 lines/way
                                        @ cache (32kB cache)
    .set WAY_FIELD_OFFSET,      30      @ start of way number field offset in
                                        @ way/index format
@
@ CP15 Control register
@
    .set CP15_CONTROL_M_MASK,       0x00000001
    .set CP15_CONTROL_C_MASK,       0x00000004
    .set CP15_CONTROL_Z_MASK,       0x00000800
    .set CP15_CONTROL_I_MASK,       0x00001000
    .set CP15_CONTROL_V_MASK,       0x00002000
    .set CP15_CONTROL_HA_MASK,      0x00020000
    .set CP15_CONTROL_EE_MASK,      0x02000000
    .set CP15_CONTROL_NMFI_MASK,    0x08000000
    .set CP15_CONTROL_TRE_MASK,     0x10000000
    .set CP15_CONTROL_AFE_MASK,     0x20000000
    .set CP15_CONTROL_TE_MASK,      0x40000000

    @AREA    MemoryManagment, CODE, READONLY
    .balign   4
    .code 32


@****************************************************************************
@  XL_EnableMmu
@
@       This function enables MMU. R0 contains MMU table address.
@
@****************************************************************************

.func XL_EnableMmu
XL_EnableMmu:

    @ Set MMU translation table base address
    MCR     p15, 0, R0, c2, c0, 0

    @ Set Domain access control register to 'client rights' for domains 0-5
    @ 'no access' for other domains
    MOV     R1, #0x550
    ADD     R1, R1, #0x5
    MCR     p15, 0, R1, c3, c0, 0@ write domain access permissions
    ISB

    @ Set FCSE Process ID register to 0
    MOV     R1, #0
    MCR     p15, 0, R1,c13, c0, 0

    @ Invalidate entire Unified TLB, R1 register ignored
    MCR     p15, 0, R0, c8, c7, 0

    @ Enable MMU
    MRC     p15,  0, R1, c1, c0 , 0     @ read MMU system control register
    ORR     R1, R1, #1                  @ set MMU enable bit
    MCR     p15,  0, R1, c1, c0 , 0     @ write MMU system control register

    @ Provide safe environment before exiting
    DSB
    ISB

    BX      LR
.endfunc


@****************************************************************************
@   XL_EnableIcache
@
@       This function enables ICache
@
@****************************************************************************
.func XL_EnableIcache
XL_EnableIcache:

    @ Invalidate entire L1 ICache
    MOV     R1, #0
    MCR     p15,  0, R1, c7, c5,  0     @ write to Cache operations register

    @ Flush entire branch target cache
    MOV     R1, #0
    MCR     p15, 0, R1, c7, c5, 6 @ write to Cache operations register

    @ Enable ICache
    MRC     p15,  0, R1, c1, c0 , 0     @ read control reg
    ORR     R1, R1, #0x1000             @ set ICache enable bit
    MCR     p15,  0, R1, c1, c0 , 0     @ write control reg

    @ wait completion of all transfers
    DSB
    ISB

    BX      LR
.endfunc


@****************************************************************************
@  XL_EnableDcache
@
@       This function enables DCache
@
@****************************************************************************
.func XL_EnableDcache
XL_EnableDcache:

    PUSH {LR}

    @ Invalidate entire DCache
    BL      XL_InvalidateDcache
    DSB

    @ Enable DCache
    MRC     p15,  0, R1, c1, c0 , 0     @ read control reg
    ORR     R1, R1, #0x04               @ set DCache enable bit
    MCR     p15,  0, R1, c1, c0 , 0     @ write control reg

    DSB
    ISB

    POP     {PC}
.endfunc


@****************************************************************************
@  XL_DisableMmu
@
@       This function disables MMU
@
@****************************************************************************
.func XL_DisableMmu
XL_DisableMmu:

    @ Read the MMU system control register
    MRC     p15, 0, R0, c1, c0, 0

    @ turn OFF the MMU
    BIC     R0, R0, #1
    MCR     p15, 0, R0, c1, c0, 0

    @ Invalidate entire Unified TLB, register R0 ignored
    @ GGG: Check if TLB invalidation is really required ? It at least ensures
    @ a valid TLB state . Is it possible to be interrupted ?
    MCR     p15, 0, R0, c8, c7, 0

    @ Synchronization barrier. Completion of TLB maintenance operation
    DSB
    ISB

    BX      LR
.endfunc


@****************************************************************************
@  XL_DisableIcache
@
@       This function disables ICache
@
@****************************************************************************
.func XL_DisableIcache
XL_DisableIcache:

    @ Disable ICache
    MRC     p15,  0, R1, c1, c0 , 0     @ read control reg
    BIC     R1, R1, #0x1000             @ set ICache enable bit
    MCR     p15,  0, R1, c1, c0 , 0     @ write control reg

    @ Invalidate entire L1 ICache
    MOV     R1, #0
    MCR     p15,  0, R1, c7, c5,  0     @ write to Cache operations register

    @ Flush entire branch target cache
    MOV     R1, #0
    MCR     p15, 0, R1, c7, c5, 6       @ write to Cache operations register

    @ wait completion of all transfers
    DSB
    ISB

    BX      LR
.endfunc

@****************************************************************************
@  XL_DisableDcache
@
@       This function disables DCache and performs cache invalidation.
@
@****************************************************************************
.func XL_DisableDcache
XL_DisableDcache:

    PUSH    {LR}

    @ Disable DCache
    MRC     p15,  0, R1, c1, c0 , 0     @ read control reg
    BIC     R1, R1, #0x04               @ set DCache enable bit
    MCR     p15,  0, R1, c1, c0 , 0     @ write control reg
    DSB

    @ Invalidate entire DCache
    BL      XL_CleanInvDcache
    DSB
    ISB

    POP     {PC}
.endfunc

@****************************************************************************
@  XL_InvalidateIcache
@
@       This function invalidates the whole ICache
@
@****************************************************************************
.func XL_InvalidateIcache
XL_InvalidateIcache:

    @ Invalidate Entire Instruction Cache
    MOV     R0, #0
    MCR     p15, 0, R0, c7, c5, 0
    DSB

    @ Flush entire branch target cache
    MOV     R1, #0
    MCR     p15, 0, R1, c7, c5, 6   @ write to Cache operations register

    DSB                         @ ensure that maintenance operations are seen
    ISB                         @ by the instructions rigth after the ISB

    BX      LR
.endfunc


@****************************************************************************
@  XL_InvalidateDcache
@
@       This function invalidates the whole DCache
@
@****************************************************************************
.func XL_InvalidateDcache
XL_InvalidateDcache:

    MOV     R0, #0                      @ write the Cache Size selection
    MCR     p15, 2, R0, c0, c0, 0       @ register to be sure we address the
    ISB                                 @ data cache ISB to sync the change to
                                        @ the CacheSizeID reg

    MOV     R0, #0                  @ set way number to 0
_inv_nextWay:
    MOV     R1, #0                  @ set line number (=index) to 0
_inv_nextLine:
    ORR     R2, R0, R1              @ construct way/index value
    MCR     p15, 0, R2, c7, c6, 2   @ invalidate by set/way
    ADD     R1, R1, #1 << 5         @ increment the index
    CMP     R1, #1 << 13            @ look for overflow out of set field
    BNE     _inv_nextLine
    ADD     R0, R0, #1 << 30        @ increment the way number
    CMP     R0, #0                  @ look for overflow out of way field
    BNE     _inv_nextWay

    DSB                             @ synchronise. ISB not
    BX      LR                      @ systematically needed?
.endfunc


@****************************************************************************
@  XL_CleanDcache
@
@       This function cleans the whole DCache
@
@****************************************************************************
.func XL_CleanDcache
XL_CleanDcache:

    MOV     R0, #0                  @ write the Cache Size selection register
    MCR     p15, 2, R0, c0, c0, 0   @ to be sure we address the data cache
    ISB                             @ ISB to sync the change to the CacheSizeID

    MOV     R0, #0                  @ set way number to 0
_cl_nextWay:
    MOV     R1, #0                  @ set line number (=index) to 0
_cl_nextLine:
    ORR     R2, R0, R1              @ construct way/index value
    MCR     p15, 0, R2, c7, c10, 2  @ clean by set/way
    ADD     R1, R1, #1 << 5         @ increment the index
    CMP     R1, #1 << 13            @ look for overflow out of set field
    BNE     _cl_nextLine
    ADD     R0, R0, #1 << 30        @ increment the way number
    CMP     R0, #0                  @ look for overflow out of way field
    BNE     _cl_nextWay

    DSB                             @ synchronise. ISB not systematically
    BX      LR                      @ needed? GGG
.endfunc


@****************************************************************************
@  XL_CleanInvDcache
@
@       This function cleans and invalidates the whole DCache
@
@****************************************************************************
.func XL_CleanInvDcache
XL_CleanInvDcache:

    MOV     R0, #0                  @ write the Cache Size selection register
    MCR     p15, 2, R0, c0, c0, 0   @ to be sure we address the data cache
    ISB                             @ ISB to sync the change to the CacheSizeID

    MOV     R0, #0                  @ set way number to 0
_cli_nextWay:
    MOV     R1, #0                  @ set line number (=index) to 0
_cli_nextLine:
    ORR     R2, R0, R1              @ construct way/index value
    MCR     p15, 0, R2, c7, c14, 2   @ invalidate by set/way
    ADD     R1, R1, #1 << 5         @ increment the index
    CMP     R1, #1 << 13            @ look for overflow out of set field
    BNE     _cli_nextLine
    ADD     R0, R0, #1 << 30        @ increment the way number
    CMP     R0, #0                  @ look for overflow out of way field
    BNE     _cli_nextWay

    DSB                             @ synchronise. ISB not systematically
                                    @ needed?
    BX      LR
.endfunc


@****************************************************************************
@  XL_ReadCp15CtrlReg
@
@       This function returns CP15 control reg
@
@****************************************************************************
.func XL_ReadCp15CtrlReg
XL_ReadCp15CtrlReg:

    MRC     p15, 0, R0, c1, c0, 0

    BX      LR
.endfunc


@****************************************************************************
@  XL_ReadCp15CtrlReg
@
@       This function returns CP15 control reg
@
@****************************************************************************
.func XL_ReadCpsr
XL_ReadCpsr:

    MRS     R0, CPSR
    BX      LR
.endfunc

@****************************************************************************
@  XL_SetStackPointer
@
@       This function set SP to address given in R0.
@
@****************************************************************************
.func XL_SetStackPointer
XL_SetStackPointer:

    MOV     SP, R0
    BX      LR

.endfunc


@****************************************************************************
@  XL_SetCpu1MmuCache
@
@    This function enable the MMU and cache to use the same value as CPU0 boot
@
@****************************************************************************
.func XL_SetCpu1MmuCache
XL_SetCpu1MmuCache:

    PUSH {R0,LR}
    @ Set TTB0 base address (0x4009C000).
    @ TTB1 is not used and TTB ctrl reg is OK
    @ REGS:
    LDR     R0, BOOT_TTBR0_ADDR
    MCR     p15, 0, R0, c2, c0, 0@ write TTB0 register

    @ Set Domain access control register to 'client rights' for domains 0-5
    @ 'no access' for other domains
    @ REGS:
    MOV     R0, #0x550
    ADD     R0, R0, #0x5
    MCR     p15, 0, R0, c3, c0, 0@ write domain access permissions
    ISB

    @ Enable the MMU
    @ Invalidate entire Unified TLB
    @ Data R0 ignored
    MCR     p15, 0, R0, c8, c7, 0
    @ Read the MMU system control register
    MRC     p15, 0, R0, c1, c0, 0
    @ turn ON the MMU
    ORR     R0, R0,#0x01 @#CP15_CONTROL_M_MASK
    MCR     p15, 0, R0, c1, c0, 0
    @ Provide safe environment to execution
    DSB
    ISB

    @ Now we can switch on the caches
    BL XL_EnableIcache

    BL XL_EnableDcache

    POP     {R0,LR}
    BX      LR
.endfunc


@****************************************************************************
@  XL_GetMMUBaseAddress
@
@    This function returns the MMU first level descriptor table base address.
@
@****************************************************************************
.func XL_GetMMUBaseAddress
XL_GetMMUBaseAddress:
    MRC     p15,0,R0,c2,c0,0
    BX      LR
.endfunc


@****************************************************************************
@  XL_SetMMUBaseAddress
@
@    This function sets the MMU first level descriptor table base address.
@   It also flush TLBs and branch target cache, it invalidates Icache and
@   clean and inv Dcache.
@
@****************************************************************************
.func XL_SetMMUBaseAddress
XL_SetMMUBaseAddress:
    PUSH    {R4,LR}

    @ Set MMU translation table base address
    MCR     p15, 0, R0, c2, c0, 0
    ISB
    @ Invalidate entire Unified TLB, R0 register ignored
    MCR     p15, 0, R0, c8, c7, 0

    @ Invalidate entire L1 ICache
    MOV     R1, #0
    MCR     p15,  0, R1, c7, c5,  0

    @ Flush entire branch target cache
    MOV     R1, #0
    MCR     p15, 0, R1, c7, c5, 6

    @ invalidate entire d cache
    BL      XL_CleanInvDcache
    ISB

    POP     {R4,PC}
.endfunc


@
@
@   void XL_InvalidateIcacheArea( void *p_start, void *p_end );
@
@ HWARM_InvalidateIcache invalidates instruction cache area whose limits are
@ given in parameters.
@ It also invalidates the BTAC.
@
@
.func XL_InvalidateIcacheArea
XL_InvalidateIcacheArea:

    CMP     R0, R1                              @ Check that end >= start.
    BHI     _inv_icache_exit                    @ Otherwise return.

    BIC     R0, R0, #0x1F                       @ Mask 5 LSBits
_inv_icache_nextLine:
    MCR     p15, 0, R0, c7, c5, 1               @ Invalidate ICache single
                                                @ entry (MVA)
    ADD     R0, R0, #1 << LINE_FIELD_OFFSET     @ Next cache line
    CMP     R1, R0
    BPL     _inv_icache_nextLine
    DSB

    @ Flush entire branch target cache
    MOV     R1, #0
    MCR     p15, 0, R1, c7, c5, 6   @ write to Cache operations register
    DSB                             @ ensure that maintenance operations
    ISB                             @ are seen by the instructions rigth
                                    @ after the ISB

_inv_icache_exit:
    BX      LR
.endfunc

@
@   void XL_InvalidateDcacheArea(void *p_start, void *p_end)
@
@ This function invalidate from data cache the memory area between p_start and
@ p_end. It execute a data sync barrier at the end.
@
@ data cache caracteristic:
@  - only 1 level of cache (level2 cache is considered as outer cache and
@    is not supported by this function).
@  - 8*4 bytes lines
@  - 0x100 lines
@  - 4 ways
.func XL_InvalidateDcacheArea
XL_InvalidateDcacheArea:

    CMP     R0, R1                      @ check that end >= start.
                                        @ Otherwise return.
    BHI     _inv_area_dcache_exit

    MOV     R2, #0                      @ write the Cache Size selection reg
    MCR     p15, 2, R2, c0, c0, 0       @ to be sure we address the data cache
    ISB                                 @ ISB to sync the change to the
                                        @ CacheSizeID reg

    @ before invalidating the Dcache, if the data cache is on and if the SP
    @ points in targeted area then we need to clean the top of Stack.
    @ if this is not done, R4 and LR that were pushed by SLAPROC_MaintainCache
    @ will be lost and return from function impossible
    MRC         p15, 0, R2, c1, c0, 0       @ read cp15 ctrl reg
    TST         R2, #CP15_CONTROL_C_MASK    @ is dcache on?
    BEQ         _inv_area_dcache_off        @ if not, bypass stack cleaning

    MOV     R2, SP                      @ read SP
    BIC     R2, #0x1F                   @ align address on 8 words boundary
    MCR     p15, 0, R2, c7, c10, 1      @ clean line containing current SP
    ADD     R2, R2, #0x20               @ clean also next line as PUSH op may
    MCR     p15, 0, R2, c7, c10, 1      @ have crossed a line limit
    DSB

_inv_area_dcache_off:
    BIC     R0, R0, #0x1F                       @ Mask 5 LSBits
_inv_area_dcache_nl:
    MCR     p15, 0, R0, c7, c6, 1               @ Invalidate DCache single
                                                @ entry (MVA)
    ADD     R0, R0, #1 << LINE_FIELD_OFFSET     @ Next cache line
    CMP     R1, R0
    BPL     _inv_area_dcache_nl

_inv_area_dcache_exit:
    DSB
    BX      LR
.endfunc

@
@   void XL_CleanDcacheArea(void *p_start, void *p_end)
@
@ This function clean from data cache the memory area between p_start and
@ p_end. It execute a data sync barrier at the end.
@
@ data cache caracteristic:
@  - only 1 level of cache (level2 cache is considered as outer cache and
@    is not supported by this function).
@  - 8*4 bytes lines
@  - 0x100 lines
@  - 4 ways
.func XL_CleanDcacheArea
XL_CleanDcacheArea:

    CMP     R0, R1                  @ check that end >= start.
    BHI     _cl_area_exit           @ Otherwise return.

    MOV     R2, #0                  @ write the Cache Size selection register
    MCR     p15, 2, R2, c0, c0, 0   @  to be sure we address the data cache
    ISB                             @ ISB to sync the change to the
                                    @ CacheSizeID reg

    BIC     R0, R0, #0x1F           @ Mask 5 LSBits
_cl_area_nextLine:
    MCR     p15, 0, R0, c7, c10, 1              @ Clean DCache single
                                                @ entry (MVA)
    ADD     R0, R0, #1 << LINE_FIELD_OFFSET     @ Next cache line
    CMP     R1, R0
    BPL     _cl_area_nextLine

_cl_area_exit:

    DSB                             @ synchronise
    BX      LR
.endfunc





@ Must be aligned on boot scatter file, MMU table location:
BOOT_TTBR0_ADDR:  .4byte  0x4009C000
