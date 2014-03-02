@
@  Copyright (C) ST-Ericsson SA 2011. All rights reserved.
@  This code is ST-Ericsson proprietary and confidential.
@  Any use of the code for whatever purpose is subject to
@  specific written permission of ST-Ericsson SA.
@
@-------------------------------------------------------------------------------
@DESCRIPTION:    Memory functions when no glibc
@-------------------------------------------------------------------------------
@-------------------------------------------------------------------------------
@

@#define MOD_TYPE        name

@ Number of bytes copied/written per lap using 4 registers
    .set WL4,	16

@ 2log(WL) = number of bits to shift when multiplying/dividing with WL4
    .set WB4,	4

@ Number of bytes copied/written per lap using 2 registers
    .set WL,	8

@ 2log(WL) = number of bits to shift when multiplying/dividing with WL
    .set WB,	3

@ Number of bytes to align accesses to
    .set WAL,	4

@ 2log(WAL) = number of bits to shift when multiplying with WAL
    .set WAB,	2

@-----------------------------------------------------------------------------

    @PRESERVE8
    .balign 8

    .global memcpy
    .global memmove


    @AREA    MemoryManagment, CODE, READONLY
    .balign   4
    .code 32

@-----------------------------------------------------------------------------
@ void *memcpy(void *s1, const void *s2, size_t n);
@-----------------------------------------------------------------------------
.func memcpy
memcpy:
memcpy_func:
  b       memcpy_asc


@-----------------------------------------------------------------------------


@-----------------------------------------------------------------------------
@ void *memcpy_asc(void *s1, const void *s2, size_t n);
@-----------------------------------------------------------------------------
memcpy_asc:
   STMDB    SP!,{R0,R4,R5,R6,LR}      	@ Save R0 for return pointer, R4 and LR
					@ for scratch

   CMP      R2,#(WL4-1)          @ Use byte copy for small chunks
   BCC      memcpy_asc_0

   AND      R3,R0,#(WAL-1)	@ Source and dest must have same alignment to
   AND      R12,R1,#(WAL-1)	@ words otherwise do byte reads
   CMP      R3,R12		@ and aligned writes
   BNE      memcpy_asc_al_dst

   CMP	    R3,#0		      @ Aligned ?
   BEQ      memcpy_asc_1

   RSB      R3,R3,#WAL		@ Not aligned start copy initial bytes
   SUB      R2,R2,R3
memcpy_asc_2:
   LDRB     R4,[R1], #+1
   STRB     R4,[R0], #+1
   SUBS     R3,R3,#+0x1
   BNE      memcpy_asc_2

memcpy_asc_1:
   AND      R3,R2, #(WL4-1)	@ Calc ending bytes to copy after word chunks
   MOVS     R2,R2, LSR #WB4	@ Calc number of word chunks
   BEQ      memcpy_asc_3

memcpy_asc_4:			@ Copy 4 word chunks
   LDMIA    R1!,{R4,R5,R6,R12}
   STMIA    R0!,{R4,R5,R6,R12}
   SUBS     R2,R2,#+0x1
   BNE      memcpy_asc_4

memcpy_asc_3:			@ Copy ev. remaining end bytes
   MOV      R2,R3		@ Remaining bytes to R2

memcpy_asc_0:			@ Copy bytes
   CMP      R2,#+0x0
   BEQ      memcpy_asc_5
memcpy_asc_7:
   LDRB     R12,[R1], #+1
   STRB     R12,[R0], #+1
   SUBS     R2,R2,#+0x1
   BNE      memcpy_asc_7
memcpy_asc_5:
   LDMIA    SP!,{R0,R4,R5,R6,PC}      @ Restore regs

@-----------------------------------------------------------------------------
@ Half word copy
@-----------------------------------------------------------------------------

@-----------------------------------------------------------------------------
@ Read bytes or half words, write 4 word chunks
@
@ When entering here, R3 contain the destination address % 4. This indicates how
@ many bytes that need to be initially copied to make the destination address
@ word aligned.

memcpy_asc_al_dst:
   RSB      R3,R3,#WAL		@ Dest not aligned start copy initial bytes
   SUB      R2,R2,R3            @ That many bytes less in main word copy loop

memcpy_asc_al_dst_ib:           @ Loop trough initial unaligned bytes
   LDRB     R4,[R1], #+1
   STRB     R4,[R0], #+1
   SUBS     R3,R3,#+0x1
   BNE      memcpy_asc_al_dst_ib

@ Now the destination address is word aligned and we can calculate
@ the number of words we can copy to R2 and keep the number of bytes
@ that will remain in R3.

   AND      R3,R2, #(WL4-1)	@ Calc ending bytes to copy after chunk copy
   MOVS     R2,R2, LSR #WB4	@ Calc number of chunks
   BEQ      memcpy_asc_3        @ No whole chunk ? -> copy remaining bytes

@ If the source address is even, we can do some half word reads...

   TST      R1,#+1
   BEQ      memcpy_asc_hw_w

@ ...otherwise we need to do some byte reads...

   TST      R1,#+2              @ Source at align+1 or align+3
   BNE      memcpy_asc_b_a3

memcpy_asc_b_a1:
   @ -:Not read, 0-3:byte of word [0-3], x:read but not used
   @
   @ 000x
   @ 1110
   @ 2221
   @ 3332
   @ ---3

   SUB      R1,R1, #+1              @ Start reading one byte early to align
				    @ for word reads (cheat !)
   LDMIA    R1!,{R4,R5,R6,LR}
   LDRB     R12,[R1],#+1            @ Read high byte of word 3

   MOV      R4,R4,LSR #0x8          @ Lower 3 bytes of word 0
   ORR      R4,R4,R5,LSL #0x18      @ Upper byte of word 0
   MOV      R5,R5,LSR #0x8          @ Lower 3 bytes of word 1
   ORR      R5,R5,R6,LSL #0x18      @ Upper byte of word 1
   MOV      R6,R6,LSR #0x8          @ Lower 3 bytes of word 2
   ORR      R6,R6,LR,LSL #0x18      @ Upper byte of word 2
   MOV      LR,LR,LSR #0x8          @ Lower 3 bytes of word 3
   ORR      LR,LR,R12,LSL #0x18     @ Upper byte of word 3

   STMIA    R0!,{R4,R5,R6,LR}       @ Write four words chunk
   SUBS     R2,R2,#+0x1             @ One chunk less to go
   BNE      memcpy_asc_b_a1
   B        memcpy_asc_3            @ Done with chunks, copy remaining bytes

memcpy_asc_b_a3:
   @ -:Not read, 0-3:byte of word [0-3], x:read but not used
   @
   @ 0---
   @ 1000
   @ 2111
   @ 3222
   @ x333

   LDRB     R4,[R1],#+1             @ Read low byte of word 0
   LDMIA    R1!,{R5,R6,LR}
   LDR      R12,[R1],#+3            @ Read 3 high bytes of word 3 (actually one
				    @ byte too many but... for sure within the
				    @ same cache line :-)

   ORR      R4,R4,R5,LSL #0x8       @ Upper 3 bytes of word 0
   MOV      R5,R5,LSR #0x18         @ Lower byte of word 1
   ORR      R5,R5,R6,LSL #0x8       @ Upper 3 bytes of word 1
   MOV      R6,R6,LSR #0x18         @ Lower byte of word 2
   ORR      R6,R6,LR,LSL #0x8       @ Upper 3 bytes of word 2
   MOV      LR,LR,LSR #0x18         @ Lower byte of word 3
   ORR      LR,LR,R12,LSL #0x8      @ Upper 3 bytes of word 3

   STMIA    R0!,{R4,R5,R6,LR}       @ Write four words chunk
   SUBS     R2,R2,#+0x1             @ One chunk less to go
   BNE      memcpy_asc_b_a3
   B        memcpy_asc_3            @ Done with chunks, copy remaining bytes

@ Half word reads and word writes

memcpy_asc_hw_w:                    @ Read half words, write chunks
   @ -:Not read, 0-3:byte of word [0-3], x:read but not used
   @
   @ 00-
   @ 1100
   @ 2211
   @ 3322
   @ --33
   LDRH     R4,[R1],#+2             @ Read low half word of word 0
   LDMIA    R1!,{R5,R6,LR}
   LDRH     R12,[R1],#+2            @ Read high half word of word 3

   ORR      R4,R4,R5,LSL #0x10      @ Upper half word of word 0
   MOV      R5,R5,LSR #0x10         @ Lower half word of word 1
   ORR      R5,R5,R6,LSL #0x10      @ Upper half word of word 1
   MOV      R6,R6,LSR #0x10         @ Lower half word of word 2
   ORR      R6,R6,LR,LSL #0x10      @ Upper half word of word 2
   MOV      LR,LR,LSR #0x10         @ Lower half word of word 3
   ORR      LR,LR,R12,LSL #0x10     @ Upper half word of word 3

   STMIA    R0!,{R4,R5,R6,LR}       @ Write chunk
   SUBS     R2,R2,#+1               @ One chunk less to go
   BNE      memcpy_asc_hw_w
   B        memcpy_asc_3            @ Done with chunks, copy remaining bytes

.endfunc

@-----------------------------------------------------------------------------


@-----------------------------------------------------------------------------
@ void *memcpy_desc(void *s1, const void *s2, size_t n);
@-----------------------------------------------------------------------------

memcpy_desc:
   STMDB    SP!,{R0,R4}        	@ Save R0 for return pointer and R4 for scratch

   ADD      R0,R0,R2		@ Start from end (descending)
   ADD      R1,R1,R2

   CMP      R2,#(WL-1)	  	@ Byte copy for small chunks
   BCC      memcpy_desc_0

   AND      R3,R0,#(WAL-1)	@ Source and dest must have same alignment
   AND      R4,R1,#(WAL-1)
   CMP      R3,R4
   BNE      memcpy_desc_20	@ otherwise try half word copy

   CMP      R3,#0
   BEQ      memcpy_desc_1
   SUB      R2,R2,R3
memcpy_desc_2:
   LDRB     R12,[R1, #-1]!	@ Some initial byte copy at the end...
   STRB     R12,[R0, #-1]!
   SUBS     R3,R3,#+0x1
   BNE      memcpy_desc_2

memcpy_desc_1:
   AND      R3,R2,#(WL-1)	@ Calc rest at the beginning
   MOVS     R2,R2, LSR #WB	@ Calc number of word chunks to copy
   BEQ      memcpy_desc_3

memcpy_desc_4:			@ Copy word chunks
   LDMDB    R1!,{R4,R12}
   STMDB    R0!,{R4,R12}
   SUBS     R2,R2,#+0x1
   BNE      memcpy_desc_4

memcpy_desc_3:			@ Copy remaining bytes at the beginning if any
   MOV      R2,R3		@ Remaining bytes in beginning to R2

memcpy_desc_0:			@ Byte copy
   CMP      R2,#0
   BEQ      memcpy_desc_5

memcpy_desc_6:
   LDRB     R12,[R1, #-1]!
   STRB     R12,[R0, #-1]!
   SUBS     R2,R2,#+0x1
   BNE      memcpy_desc_6
memcpy_desc_5:			@ Done
   LDMIA    SP!,{R0,R4}        	@ Restore regs
   BX       LR                  @ return

@-----------------------------------------------------------------------------
@ Half word copy (desc)
@-----------------------------------------------------------------------------

memcpy_desc_20:
   AND      R3,R0,#1		@ Source and dest must have same half word
   AND      R12,R1,#1		@ alignment
   CMP      R3,R12
   BNE      memcpy_desc_0	@ otherwise do byte copy

   CMP      R3,#0		@ Half word aligned ?
   BEQ      memcpy_desc_21	@ Yes, go to half word copy loop

   LDRB     R12,[R1, #-1]!	@ Copy one byte at the end...
   STRB     R12,[R0, #-1]!
   SUBS     R2,R2,#+0x1

memcpy_desc_21:
   AND      R3,R2,#1		@ Calc rest at the beginning
   MOVS     R2,R2, LSR #1	@ Calc number of word chunks to copy
   BEQ      memcpy_desc_23

memcpy_desc_24:			@ Copy half words
   LDRH     R12,[R1, #-2]!
   STRH     R12,[R0, #-2]!
   SUBS     R2,R2,#+0x1
   BNE      memcpy_desc_24

memcpy_desc_23:			@ Copy remaining byte at the beginning if any
   CMP      R3,#0
   BEQ      memcpy_desc_5
   LDRB     R12,[R1, #-1]!
   STRB     R12,[R0, #-1]!
   B        memcpy_desc_5

@   endmod
@-----------------------------------------------------------------------------

@  MOD_TYPE  crtmem_memmove
@
@  RTMODEL "ARMv4M", "USED"
@  RTMODEL "ARMv4T", "USED"
@  RTMODEL "ARMv5T", "USED"
@  RTMODEL "StackAlign4", "USED"
@  RTMODEL "__cpu_mode", "__pcs__interwork"
@  RTMODEL "__data_model", "absolute"
@  RTMODEL "__endian", "little"
@
@  PUBLIC	memmove
@  PUBLIC	memmove_func
@  extern  memcpy_asc
@  extern  memcpy_desc
@
@  RSEG	  RAMCODE_A:CODE:NOROOT(2)
@	CODE32

@-----------------------------------------------------------------------------
@ void *memmove(void *s1, const void *s2, size_t n);
@-----------------------------------------------------------------------------
.func memmove
memmove:
memmove_func:
   CMP      R1,R0
   BCS      memcpy_asc
   ADD      R12,R2,R1
   CMP      R0,R12
   BCS      memcpy_asc
   B        memcpy_desc

@   endmod

@-----------------------------------------------------------------------------


@-----------------------------------------------------------------------------
@ void *memset(void *s1, const int v, size_t n);
@-----------------------------------------------------------------------------

memset:
memset_func:
   STMDB    SP!,{R0,R4,R5}    	@ Save R0 for return pointer and R4, R5
				@ for scratch

   CMP      R2,#(WL4+1)         @ Use byte copy for small chunks
   BCC      memset_asc_0

   ANDS     R3,R0,#(WAL-1)	@ Aligned
   BEQ      memset_asc_1

   RSB      R3,R3,#WAL		@ Not aligned start with initial bytes
   SUB      R2,R2,R3
memset_asc_2:
   STRB     R1,[R0], #+1
   SUBS     R3,R3,#+0x1
   BNE      memset_asc_2

memset_asc_1:
   AND      R3,R2, #(WL4-1)	@ Calc ending bytes to copy after word chunks
   MOVS     R2,R2, LSR #WB4	@ Calc number of word chunks
   BEQ      memset_asc_3

   and      R1,R1,#+0xFF
   mov      R12,R1, LSL #0x8	@ Construct 4 dwords of all same bytes
   ADD      R1,R12,R1
   mov      R12,R1, LSL #0x10
   ADD      R1,R1,R12
   MOV      R4,R1
   MOV      R5,R1
   MOV      R12,R1

memset_asc_4:			@ Write word chunks
   STMIA    R0!,{R1,R4,R5,R12}
   SUBS     R2,R2,#+0x1
   BNE      memset_asc_4

memset_asc_3:			@ Write ev. remaining end bytes
   MOV      R2,R3		@ Remaining bytes to R2

memset_asc_0:			@ Write bytes
   CMP      R2,#+0x0
   BEQ      memset_asc_5
   STRB     R1,[R0],#+0x1
   SUBS     R2,R2,#+0x1
   BNE      memset_asc_0
memset_asc_5:
   LDMIA    SP!,{R0,R4,R5}      @ Restore regs
   BX       LR                  @ Return

@   endmod

@-----------------------------------------------------------------------------


@-----------------------------------------------------------------------------
@ int memcmp(const void *s1, const void *s2, size_t n);
@-----------------------------------------------------------------------------

memcmp:
memcmp_func:
   STMDB    SP!,{R4}        	@ Save R4 for temp result
   MOV      R4,#0		@ Equal until otherwise proven

   CMP      R2,#(2*WAL+1)	@ Use byte compare for small chunks
   BCC      memcmp_0

   AND      R3,R0,#(WAL-1)	@ Buffers must have same alignment
   AND      R12,R1,#(WAL-1)	@ otherwise do byte compare
   CMP      R3,R12
   BNE      memcmp_0

   CMP	    R3,#0		@ Aligned ?
   BEQ      memcmp_1

   RSB      R3,R3,#WAL		@ Not aligned - start compare initial bytes
   SUB      R2,R2,R3
memcmp_2:
   LDRB     R4,[R0], #+1
   LDRB     R12,[R1], #+1
   SUBS     R4,R4,R12
   BNE      memcmp_5
   SUBS     R3,R3,#+0x1
   BNE      memcmp_2

memcmp_1:
   AND      R3,R2, #(WAL-1)	@ Calc ending bytes to copy after word
   MOVS     R2,R2, LSR #WAB	@ Calc number of word
   BEQ      memcmp_3

memcmp_4:			@ Compare words
   LDR      R4,[R0], #+4
   LDR      R12,[R1], #+4
   SUBS     R4,R4,R12
   BNE      memcmp_6
   SUBS     R2,R2,#+0x1
   BNE      memcmp_4

memcmp_3:			@ Compare ev. remaining end bytes
   MOV      R2,R3		@ Remaining bytes to R2

memcmp_0:			@ Compare bytes
   CMP      R2,#+0x0
   BEQ      memcmp_5

memcmp_7:			@ Compare bytes
   LDRB     R4,[R0], #+1
   LDRB     R12,[R1], #+1
   SUBS     R4,R4,R12
   BNE      memcmp_5		@ Not equal -> Done
   SUBS     R2,R2,#+0x1
   BNE      memcmp_7
memcmp_5:
   MOV      R0,R4		@ Return value in R0
   LDMIA    SP!,{R4}         	@ Restore regs
   BX       LR                  @ Return

memcmp_6:
   SUB      R0,R0,#WAL		@ Back to start of this word
   SUB      R1,R1,#WAL
   MOV      R2,R2,LSL #WAB
   ADD      R2,R2,R3
   ADD      R2,R2,#WAL
   B        memcmp_0




@ Must be aligned on boot scatter file, MMU table location:
BOOT_TTBR0_ADDR:  .4byte  0x4009C000
