;;; Copyright ARM Ltd 2001. All rights reserved.


        EXPORT Vect
        
        PRESERVE8
        AREA Vect, CODE, READONLY

; These are example exception vectors and exception handlers
;
; Where there is ROM fixed at 0x0 (build_b), these are hard-coded at 0x0.
; Where ROM/RAM remapping occurs (build_c), these are copied from ROM to RAM.
; The copying is done automatically by the C library code inside __main.
;
; This version puts a branch to an Interrupt Handler (IRQ_Handler in int_handler.c) 
; in its exception vector table.

T_bit EQU 0x20

; *****************
; Exception Vectors
; *****************

; Note: LDR PC instructions are used here, though branch (B) instructions
; could also be used, unless the ROM is at an address >32MB.

        ENTRY
        EXPORT  Vector_Init
        EXPORT  DCC_Handler

Vector_Init

        LDR     PC, Reset_Addr
        LDR     PC, Undefined_Addr
        LDR     PC, SWI_Addr
        LDR     PC, Prefetch_Addr
        LDR     PC, Abort_Addr
        NOP                             ; Reserved vector
        LDR     PC, IRQ_Addr
        LDR     PC, FIQ_Addr

        IMPORT  SER_GIC_IntHandlerIRQ              ; In vic_services.c
        IMPORT  SER_GIC_IntHandlerFIQ              ; In vic_services.c
        IMPORT  Reset_Handler                      ; In init.s
        IMPORT  C_SWI_Handler                      ; In services.c 


Reset_Addr      DCD     Reset_Handler
Undefined_Addr  DCD     Undefined_Handler
SWI_Addr        DCD     SWI_Handler
Prefetch_Addr   DCD     Prefetch_Handler
Abort_Addr      DCD     Abort_Handler
IRQ_Addr        DCD     IRQ_Handler     
FIQ_Addr        DCD     SER_GIC_IntHandlerFIQ     ; (to be added later)

; ************************
; Exception Handlers
; ************************

; The following dummy handlers do not do anything useful in this example.
; They are set up here for completeness.

Undefined_Handler
        B       Undefined_Handler
Prefetch_Handler
        B       Prefetch_Handler
Abort_Handler
        B       Abort_Handler
; never refered to, this is only for backward compatibility purpose.
FIQ_Handler
        B       FIQ_Handler


; The following SWI handler support any SWI call with 6 parameters and 1 return value
SWI_Handler

    STMFD   sp!, {r0-r5, r12, lr}  ; Store registers
    MOV     r1, sp                 ; Set pointer to parameters
    MRS     r0, spsr               ; Get spsr
    STMFD   sp!, {r0}              ; Store spsr onto stack
    TST     r0, #T_bit             ; Occurred in Thumb state?
    LDRNEH  r0, [lr,#-2]           ; Yes: Load halfword and...
    BICNE   r0, r0, #0xFF00        ; ...extract comment field
    LDREQ   r0, [lr,#-4]           ; No: Load word and...
    BICEQ   r0, r0, #0xFF000000    ; ...extract comment field

        ; r0 now contains SWI number
        ; r1 now contains pointer to stacked registers

    BL      C_SWI_Handler          ; Call main part of handler
    CMP     r0,#1


    LDMFD   sp!, {r0}              ; Get spsr from stack
    MSR     spsr_cf, r0            ; Restore spsr
    LDMFD   sp!, {r0-r5, r12, lr}  ; Restore registers and return    
    MOVEQS pc, lr                  ; Return if SWI handled.

DCC_Handler
    MOVS   pc, lr                  ; Fall through MultiIce interface handler
    NOP

dcc_handler_space    SPACE   1024
 
        IMPORT  irq_start ; Pointers to 'C' routines.
        IMPORT irq_finish  ; from vic_services.c
_pStartIRQ DCD irq_start
_pFinishIRQ DCD irq_finish

IRQ_Handler

; We take the cycle hit of modifying the return address here,
; to simplify the code needed to return to the interrupted
; thread, possibly performing a context switch on the way.
 SUB lr, lr, #4

; At this points IRQs are disabled, so we know that the SPSR
; register will not be trashed by another interrupt.
         STMFD   sp!, {r0-r12, lr}       ; save registers
         MRS     v1, SPSR  ; push SPSR to allow nested interrupts
         STMFD   sp!, {v1}

 LDR a3, _pStartIRQ  ; Indirect pointer to StartIRQ
 LDR a2, [a3]; Check for a StartIRQ routine
 CMP a2, #0
 MOVNE lr, pc; arrange for a safe return here
 ; Ensure that this and the next instruction
 ; are not separated (either of the next options)
 ; Some generic task which is required at the start of an FIQ
   MOVNE   pc, a2 ; -- for removing warning.

;   IRQ handler
 BL SER_GIC_IntHandlerIRQ
;

 LDR a3, _pFinishIRQ; Indirect pointer to FinishIRQ
 LDR a1, [a3]; Check for a FinishIRQ routine
 CMP a1, #0
 MOVNE lr, pc; arrange for a safe return here
 ; Some generic task which is required at the end of an IRQ
   MOVNE   pc, a1;  -- for removing warning.

;; !!!! NOTE: If FinishIRQ does not return 0, jump to the returned address.
;; !!!!       So make sure your target routine matches this stack format &
;; !!!!       recovers from irq-mode properly.

 CMP a1, #0   ; a1 is non-zero for IRQs which need
 MOVNE   pc, a1;    ; to do further processing
 
 LDMFD sp!, {v1}; recover SPSR value from stack
 MSREQ SPSR_c, v1; restore the SPSR
 LDMFD sp!, {r0-r12, pc}^; Restore saved registers
     
    END

