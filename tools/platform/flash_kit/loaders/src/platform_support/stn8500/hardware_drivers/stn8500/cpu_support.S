#* -------------------------------------------------------------------------
#* $Copyright Ericsson AB 2008 $
#* The copyright to the document(s) herein is the property of Ericsson AB,
#* Sweden
#*
#* The document(s) may be used and/or copied only with the written
#* permission from Ericsson AB or in accordance with the terms and
#* conditions stipulated in the agreement/contract under which the
#* document(s) have been supplied.
#* -------------------------------------------------------------------------
#* File name  : cpu_support.s
#* File type  : ARM Assembler file
#* -------------------------------------------------------------------------
#* Description:
#*   CPU supporting functions
#* -------------------------------------------------------------------------
#* Language:    ARM Assembly with C preprocessing
#* ------------------------------------------------------------------------

        .section .text.cpusupport
        .arm

#-------------------------------------------------------------------------
#  CPU_IrqEnable()
#    Sets the I Flag to 0 in the cpu status register to allow irq interrupts
#    Used regs: R0
#    Works only in privileged mode
#-------------------------------------------------------------------------
        .global  CPU_IrqEnable
CPU_IrqEnable:
        MRS     R0, CPSR
        BIC     R0, R0, #0x80
        MSR     CPSR_c, R0

        BX      LR                      @ return

#-------------------------------------------------------------------------
# CPU_IrqDisable()
#   Sets the I Flag to 1 in the cpu status register to prevent irq interrupts
#   Used regs: R0, R1
#   Returns the previous value of the I Flag.
#   Works only in privileged mode
#-------------------------------------------------------------------------
        .global  CPU_IrqDisable
CPU_IrqDisable:
        MRS     R0, CPSR
        ORR     R1, R0, #0x80
        MSR     CPSR_c, R1
        AND     R0, R0, #0x80

        BX      LR                      @ return

#-------------------------------------------------------------------------
# CPU_ICacheEnable()
#   Sets bit 12 in the cpu status register to enable the instruction cache
#   Used regs: R0
#   Works only in privileged mode
#-------------------------------------------------------------------------
        .global  CPU_ICacheEnable
CPU_ICacheEnable:
        MRC     p15, 0, R0, c1, c0, 0    @ Read control register
        ORR     R0, R0, #0x1000          @ Set bit 12 Icache on
        MCR     p15, 0, R0, c1, c0, 0    @ Write new setting to ctl reg

        BX      LR                      @ return

#-------------------------------------------------------------------------
# CPU_BranchPredictionEnable()
#   Sets bit 11 [Z] Program flow prediction in CP15 c1 to enable branch 
#   prediction.
#   Used regs: R0
#   Works only in privileged mode
#-------------------------------------------------------------------------
        .global  CPU_BranchPredictionEnable
CPU_BranchPredictionEnable:  
        MRC     p15, 0, R0, c1, c0, 0    @ Read control register
        ORR     R0, R0, #0x800           @ Set Z bit
        MCR     p15, 0, R0, c1, c0, 0    @ Write new setting to ctl reg

        BX      LR                      @ return

#-------------------------------------------------------------------------
# CPU_UnalignAccessEnable()
#   Sets bit 22 [U] in CP15 c1 to enable unaligned data access.
#   Used regs: R0
#   Works only in privileged mode
#-------------------------------------------------------------------------
        .global CPU_UnalignAccessEnable
CPU_UnalignAccessEnable:
        MRC     p15, 0, R0, c1, c0, 0    @ Read control register
        ORR     R0, R0, #0x400000        @ Set U bit
        MCR     p15, 0, R0, c1, c0, 0    @ Write control register

        BX      LR                      @ return

#====================================================================
#        Name: PD_CPU_DCacheEnable
#  Parameters: -
#     Returns: -
# Description: Enable DCACHE.
# Notes on implementation:
#              Use ARM state.
#              R0 used as work register.
#====================================================================
        .global PD_CPU_DCacheEnable
PD_CPU_DCacheEnable:
        MRC     p15, 0, r0, c1, c0, 0     @ read control register
        ORR     r0, r0, #0x0004           @ DCACHE enable
        MCR     p15, 0, r0, c1, c0, 0     @ write control register

        BX      LR                         @ return

#====================================================================
#        Name: PD_CPU_DCacheDisable
#  Parameters: -
#     Returns: -
# Description: Disable DCACHE.
# Notes on implementation:
#              Use ARM state.
#              R0 used as work register.
#====================================================================
        .global PD_CPU_DCacheDisable
PD_CPU_DCacheDisable:  
        MRC     p15, 0, r0, c1, c0, 0      @ read control register
        LDR     r1, mask3
        AND     r0, r0, r1                 @ DCACHE disable
        MCR     p15, 0, r0, c1, c0, 0      @ write control register

        BX      LR                      @ return
mask3:
    .word 0xFFFFFFFB

#====================================================================
#        Name: PD_CPU_ICacheEnable
#  Parameters: -
#     Returns: -
# Description: Enable ICACHE.
# Notes on implementation:
#              Use ARM state.
#              R0 used as work register.
#====================================================================
        .global PD_CPU_ICacheEnable
PD_CPU_ICacheEnable:
        MRC     p15, 0, r0, c1, c0, 0    @ read control register
        ORR     r0, r0, #0x1000          @ ICACHE enable
        MCR     p15, 0, r0, c1, c0, 0    @ write control register

        BX      LR                      @ return

#====================================================================
#        Name: PD_CPU_ICacheDisable
#  Parameters: -
#     Returns: -
# Description: Disable ICACHE.
# Notes on implementation:
#              Use ARM state.
#              R0 used as work register.
#====================================================================
        .global PD_CPU_ICacheDisable
PD_CPU_ICacheDisable:  
        MRC     p15, 0, r0, c1, c0, 0   @ read control register
        BIC     r0, r0, #0x1000         @ ICACHE Disable
        MCR     p15, 0, r0, c1, c0, 0   @ write control register

        BX      LR                      @ return

#====================================================================
#        Name: PD_CPU_DataSynchBarrier
#  Parameters: -
#     Returns: -
# Description: Stall execution until all outstanding accesses in the
#              write buffer are completed.
# Notes on implementation:
#              Use ARM state.
#              R0 used as work register.
#====================================================================
        .global PD_CPU_DataSynchBarrier
PD_CPU_DataSynchBarrier:  
        MOV     r0, #0
        MCR     p15, 0, r0, c7, c10, 4  @ drain write buffer

        BX      LR                      @ return

#====================================================================
#        Name: PD_CPU_DCacheFlush
#  Parameters: -
#     Returns: -
# Description: Invalidate the whole data cache, no clean
# Notes on implementation:
#              Use ARM state.
#              R0 used as work register.
#====================================================================
        .global PD_CPU_DCacheFlush
PD_CPU_DCacheFlush:
    MOV R0, #0

way_loop:
    MOV R1, #0

line_loop:
    ORR R2,R1,R0
    MCR p15,0,R2,c7,c6,2
    ADD R1,R1,#0x40000000
    CMP R1,#0
    BNE line_loop
    ADD R0,R0,#0x00000020
    CMP R0,#0x00002000
    BNE way_loop

    BX LR

#====================================================================
#        Name: PD_CPU_DCacheInvalidateMVA
#  Parameters: MVA
#     Returns: -
# Description: Invalidate data with specified MVA
# Notes on implementation:
#              Use ARM state.
#              R0 used as work register.
#====================================================================
        .global PD_CPU_DCacheInvalidateMVA
PD_CPU_DCacheInvalidateMVA:
        BIC     r0, #15
        MCR     p15, 0, r0, c7, c6, 1   @ Flush MVA data cache

        BX      LR                      @ return

#====================================================================
#        Name: PD_CPU_ICacheFlush
#  Parameters: -
#     Returns: -
# Description: Invalidate the whole instruction cache, no clean
# Notes on implementation:
#              Use ARM state.
#              R0 used as work register.
#====================================================================
        .global PD_CPU_ICacheFlush
PD_CPU_ICacheFlush: 
        MOV     r0, #0
        MCR     p15, 0, r0, c7, c5, 0   @ Flush entire instruction cache

        BX      LR                      @ return

#====================================================================
#        Name: PD_CPU_DCacheTestClean
#  Parameters: -
#     Returns: -
# Description: Test and clean the whole data cache
# Notes on implementation:
#              Use ARM state.
#====================================================================
        .global PD_CPU_DCacheTestClean
PD_CPU_DCacheTestClean:
        MRC     p15, 0, r15, c7, c10, 3  @ test and clean
        BNE     PD_CPU_DCacheTestClean

        BX      LR                      @ return

#====================================================================
#        Name: PD_CPU_DCacheCleanInvalidate
#  Parameters: -
#     Returns: -
# Description: Clean and invalidate the whole data cache
# Notes on implementation:
#              Use ARM state.
#====================================================================
        .global PD_CPU_DCacheCleanInvalidate
        .global PD_CPU_DCacheCleanInvalidate_dirty
PD_CPU_DCacheCleanInvalidate:
        MCR     p15, 0, r0, c7, c14, 0  @ clean and invalidate
PD_CPU_DCacheCleanInvalidate_dirty:
        MRC     p15, 0, r1, c7, c10, 6  @ Test if cache dirty
        ANDS    R1, R1, #1
        BNE     PD_CPU_DCacheCleanInvalidate_dirty

        BX      LR                      @ return

#====================================================================
#        Name: PD_CPU_CacheDetails
#  Parameters: -
#     Returns: -
# Description: Returns cache details
# Notes on implementation:
#              Use ARM state.
#====================================================================
        .global PD_CPU_CacheDetails
PD_CPU_CacheDetails:
        MRC p15,0,r0,c0,c0,1@ returns cache details
        MRC p15, 0, r1, c9, c0, 0 @ Read Data Cache Lockdown Register
        MRC p15, 0, r2, c9, c0, 1 @ Read Instruction Cache Lockdown Register

        BX      LR                      @ return

#====================================================================
#        Name: PD_MMU_InvalidateTLB
#  Parameters: -
#     Returns: -
# Description: Invalidate set-associative part of TLB
# Notes on implementation:
#              Use ARM state
#              Execute from RAM
#====================================================================
        .global PD_MMU_InvalidateTLB
PD_MMU_InvalidateTLB: 
        MOV     r0, #0
        MCR     p15, 0, r0, c8, c7, 0	@ invalidate set-associative TLB

        BX      LR                      @ return

#====================================================================
#        Name: PD_CPU_Read_TTB_Address
#  Parameters: -
#     Returns: MMU TTB_Address
# Description: Reads MMU base address in CP15
#====================================================================
        .global PD_CPU_Read_TTB_Address
PD_CPU_Read_TTB_Address:
        MRC     p15, 0, r0, c2, c0, 0   @ Read TTBR

        BX      LR                      @ return

        .global	PD_CPU_SetNew_TTB_Address
PD_CPU_SetNew_TTB_Address:

        LDR     r1, mask1
        AND     r0, r0, r1              @ bits [13:5] - SBZ
        LDR     r1, opts
        ORR     r0, r0, r1              @ bits [4:0] - config

        MCR     p15, 0, r0, c2, c0, 0   @ set TTBR
        BX      LR                      @ return
mask1:
    .word 0xFFFFC01F
opts:
    .word 0x00000001        @ Write-back, no write allocate | no ECC | nonshared mem | Inner cachable
#opts    DCD 0x0000000B       @ Write-back, write allocate | no ECC | shared mem | Inner cachable

#*************************************************************************
# Description: Enable MMU to use TTB
#*************************************************************************
        .global PD_CPU_MMUEnable
PD_CPU_MMUEnable:

        LDR r1, =0x55555555         @ Put all domains in client mode in cp15 r3
        MCR p15, 0, r1, c3, c0, 0   @ Write Domain Access Control Reg (cp15 reg3)
# enable MMU
        MRC      p15, 0, r0, c1, c0, 0    @ read control register
        ORR     r0, r0, #0x0001           @ MMU enable
        MCR     p15, 0, r0, c1, c0, 0     @ write control register

        BX LR

#*************************************************************************
# Description: Disable MMU 
#*************************************************************************
        .global PD_CPU_MMUDisable
PD_CPU_MMUDisable:

# disable MMU
        MRC     p15, 0, r0, c1, c0, 0    @ read control register
        LDR     r1, mask2
        AND     r0, r0, r1               @ Disable MMU.
        MCR     p15, 0, r0, c1, c0, 0    @ write control register

        BX      LR                       @ return
mask2:
    .word 0xFFFFFFFE


#*************************************************************************
# Description: Set the value in Access Control Register(cp15 reg3)
#*************************************************************************
        .global PD_CPU_ChangeDomainAccess
PD_CPU_ChangeDomainAccess:

        MRC p15, 0, r1, c3, c0, 0 @ Read Domain Access Control Reg (cp15 reg3)
        LDR     r1, mask4         @ Erase Domain Access Control Register
        LDR     r2, mask5
        AND     r1, r0, r2        @ Add new vlaue.
        MCR p15, 0, r1, c3, c0, 0 @ Write Domain Access Control Reg (cp15 reg3)

        BX LR

mask4:
  .word 0x0
mask5:
  .word 0xFFFFFFFF

        .end
