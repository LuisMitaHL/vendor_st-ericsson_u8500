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
    EXPORT  PerfEnableDisable
	EXPORT  PerfSelectEvent
	EXPORT  PerfSetTickRate
	EXPORT  PerfGetTickRate
    EXPORT  PerfCheckForOverflow
	EXPORT  PerfReset
	EXPORT  PerfGetCount

    CODE32

;/****************************************************************************
; *  PerfEnableDisable                                                       *
; *  -----------------                                                       *
; *  Description:                                                            *
; *    enables or disables performance counters                              *
; *                                                                          *
; *  Inputs:                                                                 *
; *    r0 - 0 to disable, 1 to enable                                        *
; *                                                                          *
; *  Outputs:                                                                *
; *    see description                                                       *
; *                                                                          *
; *  Return Value:                                                           *
; *    none                                                                  *
; ****************************************************************************/
PerfEnableDisable
    MRC     p15, 0, r2, c9, c12, 0  ; Read Performance Monitor Control Register
    CMP     r0, #0
    BICEQ   r2, r2, #1 << 0         ; Clear E bit (Disable Counters)
    ORRNE   r2, r2, #1 << 0         ; Set E bit (Enable Counters)
    MCR     p15, 0, r2, c9, c12, 0  ; Write Performance Monitor Control Register
    MRCNE   p15, 0, r2, c9, c12, 1  ; Read Count Enable Set Register
    LDR     r1, =0x8000000F         ; Enable cycle counter + 4 event counters
    ORRNE   r2, r2, r1              ; Set counter enable bits
    MCRNE   p15, 0, r2, c9, c12, 1  ; Write Count Enable Set Register
    BX lr     


;/****************************************************************************
; *  PerfSelectEvent                                                         *
; *  ---------------                                                         *
; *  Description:                                                            *
; *    selects what an event counter will count                              *
; *                                                                          *
; *  Inputs:                                                                 *
; *    r0 - event counter number                                             *
; *    r1 - event number to count                                            *
; *                                                                          *
; *  Outputs:                                                                *
; *    see description                                                       *
; *                                                                          *
; *  Return Value:                                                           *
; *    none                                                                  *
; ****************************************************************************/
PerfSelectEvent
    MCR     p15, 0, r0, c9, c12, 5  ; Write Performance Counter Selection Register
    MCR     p15, 0, r1, c9, c13, 1  ; Write Event Selection Register
    BX lr


;/****************************************************************************
; *  PerfSetTickRate                                                         *
; *  ---------------                                                         *
; *  Description:                                                            *
; *    sets cycle counter tick rate to 1 or 64 cycles                        *
; *                                                                          *
; *  Inputs:                                                                 *
; *    r0 - 0 for tick rate of 1, !0 for tick rate of 64                     *
; *                                                                          *
; *  Outputs:                                                                *
; *    see description                                                       *
; *                                                                          *
; *  Return Value:                                                           *
; *    none                                                                  *
; ****************************************************************************/
PerfSetTickRate
    MRC     p15, 0, r2, c9, c12, 0  ; Read Performance Monitor Control Register
    CMP     r0, #0
    BICEQ   r2, r2, #1 << 3         ; Clear D bit (single cycle ++)
    ORRNE   r2, r2, #1 << 3         ; Set D bit (64 cycle ++)
    MCR     p15, 0, r2, c9, c12, 0  ; Write Performance Monitor Control Register
    BX lr


;/****************************************************************************
; *  PerfGetTickRate                                                         *
; *  ---------------                                                         *
; *  Description:                                                            *
; *    returns cycle counter tick rate                                       *
; *                                                                          *
; *  Inputs:                                                                 *
; *    none                                                                  *
; *                                                                          *
; *  Outputs:                                                                *
; *    none                                                                  *
; *                                                                          *
; *  Return Value:                                                           *
; *    int - cycle counter tick rate -   0 for tick rate of 1;               *
; *                                      1 for tick rate of 64               *
; ****************************************************************************/
PerfGetTickRate
    MRC     p15, 0, r0, c9, c12, 0  ; Read Performance Monitor Control Register
    LSR     r0, #3
    AND     r0, #1
    BX lr


;/****************************************************************************
; *  PerfCheckForOverflow                                                    *
; *  --------------------                                                    *
; *  Description:                                                            *
; *    checks for overflow of a performance counter                          *
; *                                                                          *
; *  Inputs:                                                                 *
; *    r0 - performance counter number:                                      *
; *                      event counter number, or >=0xff for cycle counter   *
; *                                                                          *
; *  Outputs:                                                                *
; *    none                                                                  *
; *                                                                          *
; *  Return Value:                                                           *
; *    int - 0 for no overflow, 1 for overflow                               *
; ****************************************************************************/
PerfCheckForOverflow
    MRC     p15, 0, r1, c9, c12, 3  ; Read Overflow Flag Status Register
    CMP     r0, #0xff
    LSRLT   r1, r0                  ; Shift so ECx overflow bit is bit 0
    LSRGE   r1, #31                 ; Shift so CC overflow bit is bit 0
    AND     r0, r1, #1
    BX lr


;/****************************************************************************
; *  PerfReset                                                               *
; *  ---------                                                               *
; *  Description:                                                            *
; *    resets cycle or event counters                                        *
; *                                                                          *
; *  Inputs:                                                                 *
; *    r0 - 0 for event counters, !0 for cycle counter                       *
; *                                                                          *
; *  Outputs:                                                                *
; *    see description                                                       *
; *                                                                          *
; *  Return Value:                                                           *
; *    none                                                                  *
; ****************************************************************************/
PerfReset
    MRC     p15, 0, r1, c9, c12, 0  ; Read Performance Monitor Control Register
    CMP     r0, #0                  ; Reset cycle or instruction counters
    ORREQ   r1, r1, #1 << 1         ; Set P bit (Reset event counters)
    ORRNE   r1, r1, #1 << 2         ; Set C bit (Reset cycle counter)
    MCR     p15, 0, r1, c9, c12, 0  ; Write Performance Monitor Control Register
    BX lr


;/****************************************************************************
; *  PerfGetCount                                                            *
; *  ------------                                                            *
; *  Description:                                                            *
; *    returns cycle or event counter value                                  *
; *                                                                          *
; *  Inputs:                                                                 *
; *    r0 - performance counter number:                                      *
; *                      event counter number, or >=0xff for cycle counter   *
; *                                                                          *
; *  Outputs:                                                                *
; *    none                                                                  *
; *                                                                          *
; *  Return Value:                                                           *
; *    int - the contents of thte specified performance counter              *
; ****************************************************************************/
PerfGetCount
    CMP     r0, #0xff
    MCRNE   p15, 0, r0, c9, c12, 5  ; Write Performance Counter Selection Register
    MRCNE   p15, 0, r0, c9, c13, 2  ; Read Performance Monitor Count Register
    MRCEQ   p15, 0, r0, c9, c13, 0  ; Read Cycle Count Register
    BX lr

    END
