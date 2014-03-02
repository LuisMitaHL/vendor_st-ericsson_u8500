;;; Copyright ARM Ltd 2001. All rights reserved.



; This module performs ROM/RAM remapping (if required), initializes stack 
; pointers and interrupts for each mode, and finally branches to __main in 
; the C library (which eventually calls main()).
;
; On reset, the ARM core starts up in Supervisor (SVC) mode, in ARM state, 
; with IRQ and FIQ disabled.

        PRESERVE8
        AREA    Init, CODE, READONLY
        
            EXPORT  CPU_SR_Save               ; Functions declared in   
            EXPORT  CPU_SR_Restore
            EXPORT  not_cpu0
            
; --- ensure no functions that use semihosting SWIs are linked in from the C library

;                IMPORT __use_no_semihosting_swi
        
; --- Standard definitions of mode bits and interrupt (I & F) flags in PSRs


NO_INT      EQU     0xC0                         ; Mask used to disable 

Mode_USR        EQU     0x10
Mode_FIQ        EQU     0x11
Mode_IRQ        EQU     0x12
Mode_SVC        EQU     0x13
Mode_ABT        EQU     0x17
Mode_UND        EQU     0x1B
Mode_SYS        EQU     0x1F ; available on ARM Arch 4 and later
Mode_MON        EQU     0x16 ; available on ARM 11 and later

I_Bit           EQU     0x80 ; when I bit is set, IRQ is disabled
F_Bit           EQU     0x40 ; when F bit is set, FIQ is disabled
FI_Bit          EQU     0xC0

; --- Amount of memory (in bytes) allocated for stacks

Len_FIQ_Stack    EQU     256
Len_IRQ_Stack    EQU     256
Len_ABT_Stack    EQU     256
Len_UND_Stack    EQU     256
Len_SVC_Stack    EQU     1024
Len_MON_Stack    EQU     256
Len_USR_Stack    EQU     1024


; Add lengths >0 for FIQ_Stack, ABT_Stack, UND_Stack if you need them.
; Offsets will be loaded as immediate values.
; Offsets must be 8 byte aligned.

Offset_FIQ_Stack         EQU     0
Offset_IRQ_Stack         EQU     Offset_FIQ_Stack + Len_FIQ_Stack
Offset_ABT_Stack         EQU     Offset_IRQ_Stack + Len_IRQ_Stack
Offset_UND_Stack         EQU     Offset_ABT_Stack + Len_ABT_Stack
Offset_SVC_Stack         EQU     Offset_UND_Stack + Len_UND_Stack
Offset_MON_Stack         EQU     Offset_SVC_Stack + Len_SVC_Stack
Offset_USR_Stack         EQU     Offset_MON_Stack + Len_MON_Stack

        ENTRY

        EXPORT  Reset_Handler

Reset_Handler

; ###########################################################################
        ; ##  DETECTION ID CORE AND GO STOP CORE1
        ; ###########################################################################
        MRC p15, 0, r0, c0, c0, 0    ; Read Main ID
        MRC p15, 0, r0, c0, c0, 5    ; Read MPIDR
        AND r0,#0xf                  ; Get CPU ID
        CMP r0,#0
        BNE not_cpu0                 ; if not core #0 jump to sleep state
 ;       ...                          ; go on with your single core boot


; --- Reset MMU/Caches
        MRC     p15, 0, r0, c1, c0, 0       ; read CP15 register 1 into r0
        BIC     r0, r0, #0x1                ; clear MMU enable
        BIC     r0, r0, #(0x1 << 12)        ; ensure I Cache disabled
        BIC     r0, r0, #(0x1 << 2)         ; ensure D Cache disabled
        BIC     r0, r0, #(0x1 <<3)          ; ensure Write Buffer disabled
        ORR     r0, r0, #(0x1 <<22)         ; enable U
        MCR     p15, 0, r0, c1, c0, 0       ; write value back

        MOV     R1, #0
        MOV     R3, #0x00300000
        MRC     p15, 0, R4, c1, c0, 2
        MCR     p15, 0, R3, c1, c0, 2
;       MRC     p15, 0, R4, c1, c0, 2 
        MCR     p15, 0, R1, c7, c5, 4
   
; --- Initialize stack pointer registers

; Reset the vector table 

	 	MOV     R1, #0
        
        MCR     p15,0,  R1, c12, c0, 0   ; Reset C12 (VBAR) in NS world 

; Enter each mode in turn and set up the stack pointer

        IMPORT  top_of_stacks       ; defined in stack.s and located by scatter file
        LDR     r0, =top_of_stacks

        MSR     CPSR_c, #Mode_FIQ:OR:I_Bit:OR:F_Bit ; No interrupts
        SUB     sp, r0, #Offset_FIQ_Stack

        MSR     CPSR_c, #Mode_IRQ:OR:I_Bit:OR:F_Bit ; No interrupts
        SUB     sp, r0, #Offset_IRQ_Stack

        MSR     CPSR_c, #Mode_ABT:OR:I_Bit:OR:F_Bit ; No interrupts
        SUB     sp, r0, #Offset_ABT_Stack

        MSR     CPSR_c, #Mode_UND:OR:I_Bit:OR:F_Bit ; No interrupts
        SUB     sp, r0, #Offset_UND_Stack

        MSR     CPSR_c, #Mode_SVC:OR:I_Bit:OR:F_Bit ; No interrupts
        SUB     sp, r0, #Offset_SVC_Stack


        MSR     CPSR_c, #Mode_MON:OR:I_Bit:OR:F_Bit ; No interrupts
        SUB     sp, r0, #Offset_MON_Stack   ; not used in ARM9 and before.


; --- Now change to User mode and set up User mode stack, if needed
        MSR     CPSR_c, #Mode_SYS         ; IRQs now enabled
        SUB     sp, r0, #Offset_USR_Stack
                
        IMPORT  __main

; --- Now enter the C code
        B       __main   ; note use B not BL, because an application will never return this way

CPU_SR_Save
        MRS     R0,CPSR                     ; Set IRQ and FIQ bits in CP
        ORR     R1,R0,#NO_INT
        MSR     CPSR_c,R1
        MRS     R1,CPSR                     ; Confirm that CPSR contains
        AND     R1,R1,#NO_INT
        CMP     R1,#NO_INT
        BNE     CPU_SR_Save                  ;
        BX      LR                          ; Disabled, return the origi

CPU_SR_Restore
        MSR     CPSR_c,R0
        BX      LR


; somewhere else in your text section
        ; ###########################################################################
        ; ##  STOP CORE (loops on Wait-for-interrupt)
        ; ###########################################################################
not_cpu0
        B not_cpu0

        ;EOF
        
        END

