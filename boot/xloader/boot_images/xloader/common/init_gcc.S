@
@  Copyright (C) ST-Ericsson SA 2011. All rights reserved.
@  This code is ST-Ericsson proprietary and confidential.
@  Any use of the code for whatever purpose is subject to
@  specific written permission of ST-Ericsson SA.
@
@-------------------------------------------------------------------------------
@DESCRIPTION:
@-------------------------------------------------------------------------------
@UPDATES
@ - Month day, year:
@ - Authors:
@ - Reason:
@-------------------------------------------------------------------------------
@


	@AREA   |.regsave|, ALIGN=4, DATA, NOINIT
#if (__STN_9540 > 0)
		.data @ Can't be BSS as we memset it after we have saved the registers
#else
		.bss
#endif
        .balign 4

	.global r0_save
	.global r1_save
	.global r2_save
	.global r3_save

r0_save:    .space 4
r1_save:    .space 4
r2_save:    .space 4
r3_save:    .space 4

	@AREA   |.startup|, ALIGN=4, CODE, READONLY
        .section .startup, "x"
        .balign 4
        .code 32

	@EXPORT _c_int00 [WEAK,DYNAMIC]
	.global _c_int00
	.weak _c_int00
@++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ ;
@                                 ENTRY POINT                                ;
@++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ ;


	@ENTRY

_c_int00:
	B startup	@ 0x00
	B und_handler	@ 0x04
	B swi_handler	@ 0x08
	B pabt_handler	@ 0x0c
	B dabt_handler	@ 0x10
	NOP 	        @ 0x14
	B IRQ_Handler   @ 0x18
	B FIRQ_Handler	@ 0x1c

startup:
	@b startup

	ldr r8,=r0_save
	str r0,[r8]
	ldr r8,=r1_save
	str r1,[r8]
	ldr r8,=r2_save
	str r2,[r8]
	ldr r8,=r3_save
	str r3,[r8]

@ To enable interrupts during the xloader.
	mrs r0,cpsr
	bic r0,r0, #(0x80)
	msr cpsr_cxsf, r0

	mov     r0, #0
	mov     r1, #0
	mov     r2, #0
	mov     r3, #0
	mov     r4, #0
	mov     r5, #0
	mov     r6, #0
	mov     r7, #0
	mov     r8, #0
	mov     r9, #0
	mov     r10, #0
	mov     r11, #0
	mov     r12, #0
	mov     r13, #0
	mov     r14, #0

        @ Update stack pointer to point at new stack
        ldr     sp,=linker_xloader_stack_top

	@IMPORT  __main

#if (__STN_9540 > 0)
	@ Initialize BSS
	ldr	r0, =linker_xloader_bss_start
	mov	r1, #0
	ldr	r2, =linker_xloader_bss_size
	blx	memset
#endif

@ --- Now enter the C code
	B      main   	@ note use B not BL, because an application will
			@ never return this way




@ ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ ;
@                             EXCEPTION HANDLERS                             ;
@ ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ ;


@-------------------------------;
@             SWI               ;
@-------------------------------;

swi_handler:
	B	swi_handler

@-------------------------------;
@            UNDEFINED          ;
@-------------------------------;

und_handler:
	B     und_handler

@-------------------------------;
@         PREFETCH ABORT        ;
@-------------------------------;

pabt_handler:
	B     pabt_handler

@-------------------------------;
@           DATA ABORT          ;
@-------------------------------;

dabt_handler:
	B     dabt_handler
@-------------------------------;
@           FIRQ                ;
@-------------------------------;
FIRQ_Handler:
	B     FIRQ_Handler
@-------------------------------;
@           IRQ 1               ;
@-------------------------------;
IRQ_Handler:
	B     IRQ_Handler

