 ;
 ; $Copyright:
 ; ---------------------------------------------------------------------------
 ;  This confidential and proprietary software may be used only as authorised
 ;  by a licensing agreement from ARM Limited.
 ;            (C) COPYRIGHT 2007 ARM Limited , ALL RIGHTS RESERVED
 ;  The entire notice above must be reproduced on all authorised copies and
 ;  copies may only be made to the extent permitted by a licensing agreement
 ;  from ARM Limited.
 ; ---------------------------------------------------------------------------
 ;    File:     benchmark.s
 ; ---------------------------------------------------------------------------
 ; $
 ;

; Benchmarking Code
; =================

    AREA BenchmarkingArea, CODE, READONLY

; ----------------------------------------------------------------------------
; Symbol EXPORTS
; ----------------------------------------------------------------------------
    EXPORT  PerfEnableDisable, PerfSelectEvent, PerfSetTickRate, PerfGetTickRate
    EXPORT  PerfCheckForOverflow, PerfReset, PerfGetCount
    CODE32
 
PerfEnableDisable

    MRC     p15, 0, r2, c15, c12, 0 ; Read Performance Monitor Control Register
    CMP     r0, #0
    BICEQ   r2, r2, #1 << 0         ; Clear E bit (Disable Counters)
    ORRNE   r2, r2, #1 << 0         ; Set E bit (Enable Counters)
    MCR     p15, 0, r2, c15, c12, 0 ; Write Performance Monitor Control Register
    BX lr     

















 
PerfSelectEvent
    MRC     p15, 0, r2, c15, c12, 0 ; Read Performance Monitor Control Register
    CMP     r0, #0
    LSLEQ   r1, #20                 ; Set counter 0 to count event specified in arg1
    LSLNE   r1, #12                 ; Set counter 1 to count event specified in arg1
    BICEQ   r2, r2, #0xFF << 20     ; Clear event number bits in counter event settings
    BICNE   r2, r2, #0xFF << 12
    ORR     r2, r2, r1              ; ORR event number into counter event settings
    MCR     p15, 0, r2, c15, c12, 0 ; Write Performance Monitor Control Register
    BX lr
















 
PerfSetTickRate
    MRC     p15, 0, r2, c15, c12, 0 ; Read Performance Monitor Control Register
    CMP     r0, #0
    BICEQ   r2, r2, #1 << 3         ; Clear D bit (single cycle ++)
    ORRNE   r2, r2, #1 << 3         ; Set D bit (64 cycle ++)
    MCR     p15, 0, r2, c15, c12, 0 ; Write Performance Monitor Control Register
    BX lr

















 
PerfGetTickRate
    MRC     p15, 0, r0, c15, c12, 0 ; Read Performance Monitor Control Register
    LSR     r0, #3
    AND     r0, #1
    BX lr

















 
PerfCheckForOverflow
    MRC     p15, 0, r1, c15, c12, 0 ; Read Performance Monitor Control Register
    CMP     r0, #0xff
    LSRLT   r1, r0                  ; Shift so ECx overflow bit is bit 0
    LSRLT   r1, #8
    LSRGE   r1, #10                 ; Shift so CC overflow bit is bit 0
    AND     r0, r1, #1
    BX lr
















 
PerfReset
    MRC     p15, 0, r1, c15, c12, 0 ; Read Performance Monitor Control Register
    CMP     r0, #0                  ; Reset cycle or instruction counters
    ORREQ   r1, r1, #1 << 1         ; Set P bit (Reset event counters)
    ORRNE   r1, r1, #1 << 2         ; Set C bit (Reset cycle counter)
    MCR     p15, 0, r1, c15, c12, 0 ; Write Performance Monitor Control Register
    BX lr

















 
PerfGetCount
    CMP     r0, #0                 
    MRCEQ   p15, 0, r1, c15, c12, 2 ; Read CP15 EC0 Register
    CMP     r0, #1                 
    MRCEQ   p15, 0, r1, c15, c12, 3 ; Read CP15 EC1 Register
    CMP     r0, #0xff              
    MRCEQ   p15, 0, r1, c15, c12, 1 ; Read CP15 CC Register
    MOV     r0, r1
    BX lr

    END
