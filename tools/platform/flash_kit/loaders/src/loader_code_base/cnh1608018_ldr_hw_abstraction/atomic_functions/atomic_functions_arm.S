# * -------------------------------------------------------------------------
# * $Copyright ST-Ericsson 2011 $
# * -------------------------------------------------------------------------
# * File name  : atomic_functions_arm.s
# * File type  : ARM Assembler file
# * -------------------------------------------------------------------------
# * Description:
# *   Atomic functions implementation file
# * -------------------------------------------------------------------------
# *
# * -------------------------------------------------------------------------
# * Language:    ARM Assembly
# * -------------------------------------------------------------------------

        .section .text.atomic
        .arm

#----------------------------------------------------------------------------
######## Do_Atomic_CompareAndSwap
#                      # r0 points to the value to update.
#                      # r1 contains the expected value.
#                      # r2 contains the desired value.
#                      # on exit r0 contains the original value of [r0]
####     Warning: This implementation does not insert memory barriers.
#----------------------------------------------------------------------------
        .global Do_Atomic_CompareAndSwap

Do_Atomic_CompareAndSwap:
        PUSH {r4}
        MOV r3, r0
        MOV r4, #2
try:
        LDR r0, [r3]   @ Load the value to update
        CMP r0, r1     @ Compare it to the expected value
        STREQ r2, [r3] @ Store the desired value if the expected value and the loaded value were equal
        CMP r4, #1     @ Did the store succeed?
        BEQ try        @ If not, try again, otherwise return.
        POP {r4}
        BX lr

        .end

