/* based on wmm provided files */

.extern main

.section .init,"x","progbits"
.global Reset_Handler
.align 2

.equ Mode_USR, 0x10
.equ Mode_FIQ, 0x11
.equ Mode_IRQ, 0x12
.equ Mode_SVC, 0x13
.equ Mode_ABT, 0x17
.equ Mode_UND, 0x1B
.equ Mode_SYS, 0x1F @ available on ARM Arch 4 and later
.equ Mode_MON, 0x16 @ available on ARM 11 and later
.equ I_Bit, 0x80 @ when I bit is set, IRQ is disabled
.equ F_Bit, 0x40 @ when F bit is set, FIQ is disabled
.equ FI_Bit, 0xC0

# --- Amount of memory (in bytes) allocated for stacks

.equ Len_FIQ_Stack, 256
.equ Len_IRQ_Stack,1024
.equ Len_ABT_Stack, 256
.equ Len_UND_Stack, 256
.equ Len_SVC_Stack,2048
.equ Len_MON_Stack, 256
.equ Len_USR_Stack,2048

# Add lengths >0 for FIQ_Stack, ABT_Stack, UND_Stack if you need them.
# Offsets will be loaded as immediate values.
# Offsets must be 8 byte aligned.

.equ Offset_FIQ_Stack, 0
.equ Offset_IRQ_Stack, Offset_FIQ_Stack + Len_FIQ_Stack
.equ Offset_ABT_Stack, Offset_IRQ_Stack + Len_IRQ_Stack
.equ Offset_UND_Stack, Offset_ABT_Stack + Len_ABT_Stack
.equ Offset_SVC_Stack, Offset_UND_Stack + Len_UND_Stack
.equ Offset_MON_Stack, Offset_SVC_Stack + Len_SVC_Stack
.equ Offset_USR_Stack, Offset_MON_Stack + Len_MON_Stack

.func Reset_Handler
.extern main
.align 2
Reset_Handler:

#CP15DISABLE_init ROUT
#Disabling write access to CP15

#        LDR     r0, = 0xA03DC000             @ set r0 with              ess
#        LDR     r1, = 0x21
#        STR     r1, [r0, #0x808]            @ clear TZPROT0 bit 0


# ###########################################################################
# ##  DETECTION ID CORE AND GO STOP CORE1
# ###########################################################################
        MRC p15, 0, r0, c0, c0, 0    @ Read Main ID
        MRC p15, 0, r0, c0, c0, 5    @ Read MPIDR
        AND r0,#0xf                  @ Get CPU ID
        CMP r0,#0
        BNE not_cpu0                 @ if not core #0 jump to sleep state
#       ...                          @ go on with your single core boot

	#write in to the enable clear register
        LDR     R0, = 0xA0411084

        LDR     R1, [r0]
        ORR     R1, R1, #(0x1 << 11)
        STR     R1, [R0]

      # Set the priority mask
        LDR     R0, = 0xA0410104
        LDR     R1, [r0]
        ORR     R1, R1, #(0xF8)
        STR     R1, [R0]


		# Set the security register
        LDR     R0, = 0xA0411084
        LDR     R1, [r0]
        LDR     R1,= 0xFFFFFFFF
        STR     R1, [R0]


		# Set the security register
        LDR     R0, = 0xA0411088
        LDR     R1, [r0]
        LDR     R1,= 0xFFFFFFFF
        STR     R1, [R0]


        MOV     R1, #0
        MCR     p15,0,  R1, c12, c0, 0   @ Reset C12 (VBAR) in NS world

# Setup stackpointer in all modes
	LDR r0, .stack

	MSR     CPSR_c, #Mode_FIQ|I_Bit|F_Bit @ No interrupts
	SUB     sp, r0, #Offset_FIQ_Stack

	MSR     CPSR_c, #Mode_IRQ|I_Bit|F_Bit @ No interrupts
	SUB     sp, r0, #Offset_IRQ_Stack

	MSR     CPSR_c, #Mode_ABT|I_Bit|F_Bit @ No interrupts
	SUB     sp, r0, #Offset_ABT_Stack

	MSR     CPSR_c, #Mode_UND|I_Bit|F_Bit @ No interrupts
	SUB     sp, r0, #Offset_UND_Stack

	MSR     CPSR_c, #Mode_SVC|I_Bit|F_Bit @ No interrupts
	SUB     sp, r0, #Offset_SVC_Stack

	MSR     CPSR_c, #Mode_MON|I_Bit|F_Bit @ No interrupts
	SUB     sp, r0, #Offset_MON_Stack

# --- Now change to User mode and set up User mode stack, if needed
	MSR     CPSR_c, #Mode_SYS|I_Bit|F_Bit @ No interrupts
	SUB     sp, r0, #Offset_USR_Stack

# clear bss area

	LDR r0, .bssstart
	LDR r1, .bssend
	MOV r2, #0

clearnext:
	STR r2, [r0]
	ADD r0,r0,#4
	CMP r0,r1
	BLT clearnext

#Jump to main
	MOV R0, #0
	MOV R1, #0

	b main

#would something make it return from main then loop here forevere
infinity:
	b infinity

.align 2
.stack:
	.word __stack
.bssstart:
	.word __bss_start
.bssend:
	.word __bss_end
.endfunc

###########################################################################
##  STOP CORE (loops on Wait-for-interrupt)
###########################################################################
.func not_cpu0
.align 2
not_cpu0:
        B not_cpu0
.endfunc
.end
