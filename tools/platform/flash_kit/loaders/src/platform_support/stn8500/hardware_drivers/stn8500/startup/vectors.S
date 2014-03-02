/* based on wmm provided files */

# start
.section .vectors, "x","progbits"
.global vectors
.extern SER_GIC_IntHandlerFIQ
.extern Reset_Handler
.extern Undefined_Handler
.extern SWI_Handler
.extern Prefetch_Handler
.extern Abort_Handler
.extern IRQ_Handler

.align 2
vectors:
	LDR PC, Reset_Addr
	LDR PC, Undefined_Addr
	LDR PC, SWI_Addr
	LDR PC, Prefetch_Addr
	LDR PC, Abort_Addr
	NOP						@ Reserved vector
	LDR PC, IRQ_Addr
	LDR PC, FIQ_Addr

.align 2
Reset_Addr:
	.word     Reset_Handler
Undefined_Addr:
	.word     Undefined_Handler
SWI_Addr:
	.word     SWI_Handler
Prefetch_Addr:
	.word     Prefetch_Handler
Abort_Addr:
	.word     Abort_Handler
IRQ_Addr:
	.word     IRQ_Handler
FIQ_Addr:
	.word     SER_GIC_IntHandlerFIQ

.end
