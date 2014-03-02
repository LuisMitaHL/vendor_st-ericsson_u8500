.global Undefined_Handler
.global Prefetch_Handler
.global Abort_Handler
.global SWI_Handler
.global IRQ_Handler

.extern UART_PrintCrash
.extern SER_GIC_IntHandlerIRQ

.equ T_bit, 0x20
.equ NO_IRQ, 0xC0

.ifdef ENABLE_DEBUG
.section .handlers_Undefined_Handler, "x","progbits"
.align 2
.func Undefined_Handler
Undefined_Handler:
#save before modification
	MRS	r2, SPSR
	MRS     r4, CPSR

	MOV	r3, lr

#... now modify
	ORR     r5, r2, #NO_IRQ
        MSR     CPSR_c, r5

#... this parameter goes here
	SUB	sp, sp, #12
	STR	r4, [sp, #0]

# all other parameters
	MOV	r0, #1
	MOV	r1, sp

	blx	UART_PrintCrash
	add	sp, sp, #12
	b Undefined_Handler
.endfunc

.section .handlers_Prefetch_Handler, "x","progbits"
.align 2
.func Prefetch_Handler
Prefetch_Handler:
	#save before modification
	MRS	r2, SPSR
	MRS     r4, CPSR

	MOV	r3, lr

#... now modify
	ORR     r5, r2, #NO_IRQ
        MSR     CPSR_c, r5

#... this parameter goes here
	SUB	sp, sp, #12
	STR	r4, [sp, #0]

# all other parameters
	MOV	r0, #2
	MOV	r1, sp

	blx	UART_PrintCrash
	add	sp, sp, #12
	b Prefetch_Handler
.endfunc

.section .handlers_Abort_Handler, "x","progbits"
.align 2
.func Abort_Handler
Abort_Handler:
#save before modification
	MRS	r2, SPSR
	MRS     r4, CPSR

	MOV	r3, lr

#... now modify
	ORR     r5, r2, #NO_IRQ
        MSR     CPSR_c, r5

#... this parameter goes here
	SUB	sp, sp, #12
	STR	r4, [sp, #0]

# all other parameters
	MOV	r0, #3
	MOV	r1, sp

	blx	UART_PrintCrash
	add	sp, sp, #12
	b Abort_Handler
.endfunc

.section .handlers_SWI_Handler, "x","progbits"
.align 2
.func SWI_Handler
SWI_Handler:
  B       SWI_Handler
.endfunc
.else
#No debug case
.section .handlers_Undefined_Handler, "x","progbits"
.align 2
.func Undefined_Handler
Undefined_Handler:
	B       Undefined_Handler
.endfunc
.section .handlers_Prefetch_Handler, "x","progbits"
.align 2
.func Prefetch_Handler
Prefetch_Handler:
	B       Prefetch_Handler
.endfunc
.section .handlers_Abort_Handler, "x","progbits"
.align 2
.func Abort_Handler
Abort_Handler:
	B       Abort_Handler
.endfunc
.section .handlers_SWI_Handler, "x","progbits"
.align 2
.func SWI_Handler
SWI_Handler:
  B       SWI_Handler
.endfunc
.endif

.section .handlers_IRQ_Handler,"x","progbits"
.align 2
.func IRQ_Handler
IRQ_Handler:

# We take the cycle hit of modifying the return address here,
# to simplify the code needed to return to the interrupted
# thread, possibly performing a context switch on the way.
 SUB lr, lr, #4

# At this points IRQs are disabled, so we know that the SPSR
# register will not be trashed by another interrupt.
	 STMFD   sp!, {r0-r12, lr}       @ save registers
	 MRS     v1, SPSR  @ push SPSR to allow nested interrupts
	 STMFD   sp!, {v1}

 LDR a3, _pStartIRQ  @ Indirect pointer to StartIRQ
 LDR a2, [a3] @ Check for a StartIRQ routine
 CMP a2, #0
 MOVNE lr, pc @ arrange for a safe return here
# Ensure that this and the next instruction
# are not separated (either of the next options)
# Some generic task which is required at the start of an FIQ
   MOVNE   pc, a2 @ -- for removing warning.

#  IRQ handler
 BL SER_GIC_IntHandlerIRQ

 LDR a3, _pFinishIRQ @ Indirect pointer to FinishIRQ
 LDR a1, [a3] @ Check for a FinishIRQ routine
 CMP a1, #0
 MOVNE lr, pc @ arrange for a safe return here
 @ Some generic task which is required at the end of an IRQ
   MOVNE   pc, a1 @  -- for removing warning.

# !!!! NOTE: If FinishIRQ does not return 0, jump to the returned address.
# !!!!       So make sure your target routine matches this stack format &
# !!!!       recovers from irq-mode properly. */

 CMP a1, #0   @ a1 is non-zero for IRQs which need
 MOVNE   pc, a1  @ to do further processing

 LDMFD sp!, {v1} @ recover SPSR value from stack
 MSREQ SPSR_c, v1 @ restore the SPSR
 LDMFD sp!, {r0-r12, pc}^ @ Restore saved registers

.align 2
_pStartIRQ:
    .word irq_start
_pFinishIRQ:
    .word irq_finish
.endfunc

.weak irq_start
.weak irq_finish
.end
