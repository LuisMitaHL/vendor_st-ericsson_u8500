.arch armv7-a
.fpu neon
.text
.align 4
//////////////////////////////
////box_mean_uint8_row_y_neon
//////////////////////////////
////r0:p_src
////r1:p_dst
////r2:width
////r3:height
////r4:row_idx
////r5:col_idx
////r6:src_wjump = src_wstride - width
////r7:dst_wjump = (dst_wstride - width)<<1
///////////////////////////////
.globl  _box_mean_uint8_row_y_neon
_box_mean_uint8_row_y_neon:
////#0 INITIATE SOME REGISTER
    push    {r4-r7} @reserve the r4-r7
    vpush   {q4-q7} @reserve the q4-q7
////#0 r2=width, r3=height, r6=src_wstride-width, r7=(dst_wstride-width)<<1
    ldr r6, [r3, #4]
    ldr r7, [r3, #8]
    ldr r3, [r3, #0]

    sub r6, r6, r2
    sub r7, r7, r2
    lsl r7, r7, #1
////#1 start to col loop
    mov r5, #0
    pld [r0, #128]
_BRFU8Neon_HeiLoop:
    vld1.8  {d1}, [r0,:64]!
    pld [r0, #128]
    mov r4, #8
    vdup.8  d0, d1[0]
////#1.1 width loop
_BRFU8Neon_WidLoop: 
////#1.2load d2
    vld1.8 {d2}, [r0,:64]!
    pld [r0,#128]
    cmp r4, r2
blt _BRFU8NW_Next
    vdup.8  d2, d1[7]
    sub r0, r0, #8
_BRFU8NW_Next:
    vext.8  d4, d0, d1, #4
    vext.8  d5, d0, d1, #5
    vext.8  d6, d0, d1, #6
    vext.8  d7, d0, d1, #7
    vext.8  d8, d1, d2, #1
    vext.8  d9, d1, d2, #2
    vext.8  d10, d1, d2, #3
    
    vaddl.u8    q2, d4, d5
    vaddl.u8    q3, d6, d7
    vaddl.u8    q5, d9, d10
    vaddl.u8    q4, d1, d8

    vadd.u16    q2, q2, q3
    vadd.u16    q4, q4, q5

    vadd.u16    q2, q2, q4
    vst1.8  {q2},   [r1,:128]!
    vmov    d0, d1
    vmov    d1, d2

    add r4, r4, #8
    cmp r4, r2
    ble _BRFU8Neon_WidLoop
    add r0, r0, r6
    add r1, r1, r7
    add r5, r5, #1
    cmp r5, r3
    blt _BRFU8Neon_HeiLoop
    vpop {q4-q7}
    pop {r4-r7}
    bx  lr

//////////////////////////////
////box_mean_uint8_col_y_neon
//////////////////////////////
////r0:p_src
////r1:p_dst
////r2:width
////r3:height
////r4:row_idx
////r5:col_idx
////r6:src_wjump = src_wstride<<1
////r7:dst_wjump = dst_wstride
////r8:src_hjump = (src_wstride*height-8)<<1
////r9:dst_hjump = (dst_wstride*height-8)
////r10:col_idx_1 = col_idx+3
///////////////////////////////
.globl  _box_mean_uint8_col_y_neon
_box_mean_uint8_col_y_neon:
////#0 INITIATE SOME REGISTER
    push    {r4-r10} @reserve the r4-r8
    vpush   {q4-q7} @reserve the q4-q7
////#0 r8=src_hjump, r9 = dst_hjump, r6=src_wjump, r7=dst_wjump
    ldr r6, [r3, #4]
    ldr r7, [r3, #8]
    ldr r3, [r3, #0]

    mul r8, r6, r3
    mul r9, r7, r3
    lsl r6, r6, #1
    
    sub r8, r8, #8
    sub r9, r9, #8
    lsl r8, r8, #1
////#1 start to width loop
    vmov.u16    q10, #32
    mov r4, #0
.BCFU8Neon_WidLoop:
    vld1.16  {q5}, [r0,:128]
    add r0, r0, r6
    vld1.16  {q6}, [r0,:128]
    add r0, r0, r6
    vld1.16  {q7}, [r0,:128]
    add r0, r0, r6
    pld [r0,#128]
    vmov    q1, q5
    vmov    q2, q5
    vmov    q3, q5
    vmov    q4, q5
    vshl.u16    q0, q5, #2
    vadd.u16    q0, q0, q5
    vadd.u16    q0, q0, q6
    vadd.u16    q0, q0, q7
    vmov.u16    q9, #0
    mov r5, #0
    mov r10, #3
////#1.1 height loop
.BCFU8Neon_HeiLoop:
////#1.2load q8
    cmp r10, r3
    blt .L1
    vmov    q8, q7
    b   .L2
.L1:
    vld1.16 {q8}, [r0,:128]
    add r0, r0, r6
    pld [r0,#128]    
    add r10, r10, #1
.L2:
    vadd.u16    q0, q0, q8
    vsub.u16    q0, q0, q9
    vadd.u16    q11, q0, q10
    vshrn.u16   d22, q11, #6
    vst1.8  {d22}, [r1,:64]
    add r1, r1, r7

    vmov    q9, q1
    vmov    q1, q2
    vmov    q2, q3
    vmov    q3, q4
    vmov    q4, q5
    vmov    q5, q6
    vmov    q6, q7
    vmov    q7, q8

    add r5, r5, #1
    cmp r5, r3
    blt .BCFU8Neon_HeiLoop

    sub r0, r0, r8
    sub r1, r1, r9

    add r4, r4, #8
    cmp r4, r2
    blt .BCFU8Neon_WidLoop
    vpop {q4-q7}    
    pop {r4-r10}
    bx  lr

//////////////////////////////
////box_mean_uint16_row_y_neon
//////////////////////////////
////r0:p_src
////r1:p_dst
////r2:width
////r3:height
////r4:row_idx
////r5:col_idx
////r6:src_wjump = (src_wstride - width)<<1
////r7:dst_wjump = (dst_wstride - width)<<1
///////////////////////////////
.globl  _box_mean_uint16_row_y_neon
_box_mean_uint16_row_y_neon:
////#0 INITIATE SOME REGISTER
    push    {r4-r7} @reserve the r4-r7
    vpush   {q4-q7} @reserve the q4-q7
////#0 r6=(src_wstride-width)<<1 r7 = (dst_wstride - width)<<1
    ldr r6, [r3, #4]
    ldr r7, [r3, #8]
    ldr r3, [r3, #0]

    sub r6, r6, r2
    sub r7, r7, r2
    lsl r6, r6, #1
    lsl r7, r7, #1
////#1 start to col loop
    mov r5, #0
    pld [r0, #128]
    vmov.u32    q13, #4
.BRFU16Neon_HeiLoop:
    vld1.16  {q1}, [r0,:128]!
    pld [r0, #128]
    mov r4, #8
    vdup.16  d1, d2[0]
////#1.1 width loo
.BRFU16Neon_WidLoop:
////#1.2load d2
    vld1.16 {q2}, [r0,:128]!
    pld [r0,#128]
    cmp r4, r2
    blt .BRFU16NW_Next
    vdup.16  d4, d3[3]
    sub r0, r0, #16
.BRFU16NW_Next:
    vext.8  q3, q0, q1, #8
    vext.8  q4, q0, q1, #10
    vaddl.u16   q5, d6, d8
    vaddl.u16   q6, d7, d9

    vext.8  q3, q0, q1, #12
    vext.8  q4, q0, q1, #14
    vaddl.u16   q7, d6, d8
    vaddl.u16   q8, d7, d9

    vext.8  q4, q1, q2, #2
    vaddl.u16   q9, d2, d8
    vaddl.u16   q10, d3, d9

    vext.8  q3, q1, q2, #4
    vext.8  q4, q1, q2, #6
    vaddl.u16   q11, d6, d8
    vaddl.u16   q12, d7, d9

    vadd.u32    q5, q5, q7
    vadd.u32    q6, q6, q8
    vadd.u32    q9, q9, q11
    vadd.u32    q10, q10, q12

    vadd.u32    q5, q5, q9
    vadd.u32    q6, q6, q10

    vadd.u32    q5, q5, q13
    vadd.u32    q6, q6, q13

    vshrn.u32   d10, q5, #3
    vshrn.u32   d11, q6, #3
    vst1.16  {q5},   [r1,:128]!
    vmov    d1, d3
    vmov    q1, q2

    add r4, r4, #8
    cmp r4, r2
    ble .BRFU16Neon_WidLoop
    add r0, r0, r6
    add r1, r1, r7
    add r5, r5, #1
    cmp r5, r3
    blt .BRFU16Neon_HeiLoop
    vpop {q4-q7}
    pop {r4-r7}
    bx  lr

//////////////////////////////
////box_mean_uint16_col_y_neon
//////////////////////////////
////r0:p_src
////r1:p_dst
////r2:width
////r3:height
////r4:row_idx
////r5:col_idx
////r6:src_wjump = src_stride << 1
////r7:dst_wjump = dst_stride << 1
////r8:src_hjump = (src_wstride*height-8)<<1
////r9:dst_hjump = (dst_wstride*height-8)<<1
////r10:col_idx_1 = col_idx+3
///////////////////////////////
.globl  _box_mean_uint16_col_y_neon
_box_mean_uint16_col_y_neon:
////#0 INITIATE SOME REGISTER
    push    {r4-r10} @reserve the r4-r10
    vpush   {q4-q7} @reserve the q4-q7
////#0 r8=src_hjump, r9 = dst_hjump, r6=src_wjump, r7=dst_wjump
    ldr r6, [r3, #4]
    ldr r7, [r3, #8]
    ldr r3, [r3, #0]

    mul r8, r6, r3
    mul r9, r7, r3
    lsl r6, r6, #1
    lsl r7, r7, #1

    sub r8, r8, #8
    sub r9, r9, #8
    lsl r8, r8, #1
    lsl r9, r9, #1
    ////#1 start to width loop
    vmov.u32    q11, #4
    mov r4, #0
.BCFU16Neon_WidLoop:
    vld1.16  {q5}, [r0,:128]
    add r0, r0, r6
    vld1.16  {q6}, [r0,:128]
    add r0, r0, r6
    vld1.16  {q7}, [r0,:128]
    add r0, r0, r6
    pld [r0,#128]
    vmov    q1, q5
    vmov    q2, q5
    vmov    q3, q5
    vmov    q4, q5
    vaddl.u16   q0, d2, d4
    vaddw.u16   q0, q0, d6
    vaddw.u16   q0, q0, d8
    vaddw.u16   q0, q0, d10
    vaddw.u16   q0, q0, d12
    vaddw.u16   q0, q0, d14

    vaddl.u16   q10, d3, d5
    vaddw.u16   q10, q10, d7
    vaddw.u16   q10, q10, d9
    vaddw.u16   q10, q10, d11
    vaddw.u16   q10, q10, d13
    vaddw.u16   q10, q10, d15

    vmov.u16    q9, #0
    mov r5, #0
    mov r10, #3
////#1.1 height loop
.BCFU16Neon_HeiLoop:
////#1.2load q8
    cmp r10, r3
    blt .L3
    vmov    q8, q7
    b   .L4
.L3:
    vld1.16 {q8}, [r0,:128]
    add r0, r0, r6
    pld [r0,#128]    
    add r10, r10, #1
.L4:
    vaddw.u16   q0, q0, d16
    vaddw.u16   q10, q10, d17
    vsubw.u16   q0, q0, d18
    vsubw.u16   q10, q10, d19

    vadd.u32    q12, q0, q11
    vadd.u32    q13, q10, q11
    vshrn.u32   d24, q12, #3
    vshrn.u32   d25, q13, #3
    vst1.16  {q12}, [r1,:128]
    add r1, r1, r7

    vmov    q9, q1
    vmov    q1, q2
    vmov    q2, q3
    vmov    q3, q4
    vmov    q4, q5
    vmov    q5, q6
    vmov    q6, q7
    vmov    q7, q8

    add r5, r5, #1
    cmp r5, r3
    blt .BCFU16Neon_HeiLoop
    sub r0, r0, r8
    sub r1, r1, r9

    add r4, r4, #8
    cmp r4, r2
    blt .BCFU16Neon_WidLoop
    vpop {q4-q7}
    pop {r4-r10}
    bx  lr

//////////////////////////////
////box_mean_uint8_row_uv_neon
//////////////////////////////
////r0:p_src
////r1:p_dst
////r2:width
////r3:height
////r4:row_idx
////r5:col_idx
////r6:src_wjump = src_wstride - width
////r7:dst_wjump = (dst_wstride - width)<<1
///////////////////////////////
.globl  _box_mean_uint8_row_uv_neon
_box_mean_uint8_row_uv_neon:
////#0 INITIATE SOME REGISTER
    push    {r4-r7} @reserve the r4-r6
    vpush   {q4-q7} @reserve the q4-q7
////#0 r2=width, r3=height, r6=src_wstride-width, r7=(dst_wstride-width)<<1
    ldr r6, [r3, #4]
    ldr r7, [r3, #8]
    ldr r3, [r3, #0]

    sub r6, r6, r2
    sub r7, r7, r2
    lsl r7, r7, #1
////#1 start to col loop
    mov r5, #0
    pld [r0, #128]
_BRFU8UVNeon_HeiLoop:
    vld1.8  {q1}, [r0,:128]!
    pld [r0, #128]
    mov r4, #16
    vdup.16 d1, d2[0]
////#1.1 width loop
_BRFU8UVNeon_WidLoop:
////#1.2load d2
    vld1.8 {q2}, [r0,:128]!
    pld [r0,#128]
    cmp r4, r2
    blt _BRFU8UVNW_Next
    vdup.16  d4, d3[3]
    sub r0, r0, #16
_BRFU8UVNW_Next:
    vext.8  q3, q0, q1, #12
    vext.8  q4, q0, q1, #14
    vext.8  q5, q1, q2, #2

    vaddl.u8    q6, d6, d8
    vaddl.u8    q7, d7, d9
    vaddl.u8    q8, d2, d10
    vaddl.u8    q9, d3, d11

    vadd.u16    q6, q6, q8
    vadd.u16    q7, q7, q9

    vst1.16 {q6,q7},    [r1,:128]!
    vmov    d1, d3
    vmov    q1, q2

    add r4, r4, #16
    cmp r4, r2
    ble _BRFU8UVNeon_WidLoop
    add r0, r0, r6
    add r1, r1, r7
    add r5, r5, #1
    cmp r5, r3
    blt _BRFU8UVNeon_HeiLoop
    vpop {q4-q7}
    pop {r4-r7}
    bx  lr

//////////////////////////////
////box_mean_uint8_col_uv_neon
//////////////////////////////
////r0:p_src
////r1:p_dst
////r2:width
////r3:height
////r4:row_idx
////r5:col_idx
////r6:src_wjump = src_wstride<<1
////r7:dst_wjump = dst_wstride
////r8:src_hjump = (src_wstride*height-8)<<1
////r9:dst_hjump = (dst_wstride*height-8)
////r10:col_idx_1 = col_idx+1
///////////////////////////////
.globl  _box_mean_uint8_col_uv_neon
_box_mean_uint8_col_uv_neon:
////#0 INITIATE SOME REGISTER
    push    {r4-r10} @reserve the r4-r9
    vpush   {q4-q7} @reserve the q4-q7
////#0 r8=src_hjump, r9 = dst_hjump, r6=src_wjump, r7=dst_wjump
    ldr r6, [r3, #4]
    ldr r7, [r3, #8]
    ldr r3, [r3, #0]

    mul r8, r6, r3
    mul r9, r7, r3
    lsl r6, r6, #1

    sub r8, r8, #8
    sub r9, r9, #8
    lsl r8, r8, #1
    ////#1 start to width loop
    vmov.u16    q10, #8
    mov r4, #0
.BCFU8UVNeon_WidLoop:
    vld1.16  {q5}, [r0,:128]
    add r0, r0, r6
    pld [r0,#128]
    vmov    q3, q5
    vmov    q4, q5
    vshl.u16    q0, q5, #1
    vadd.u16    q0, q0, q5
    vmov.u16    q9, #0
    mov r5, #0
    mov r10, #1
////#1.1 height loop
.BCFU8UVNeon_HeiLoop:
////#1.2load q8
    cmp r10, r3
    blt .LUV1
    vmov    q6, q5
    b   .LUV2
.LUV1:
    vld1.16 {q6}, [r0,:128]
    add r0, r0, r6
    pld [r0,#128]    
    add r10, r10, #1
.LUV2:
    vadd.u16    q0, q0, q6
    vsub.u16    q0, q0, q9
    vadd.u16    q11, q0, q10
    vshrn.u16   d22, q11, #4
    vst1.8  {d22}, [r1,:64]
    add r1, r1, r7

    vmov    q9, q3
    vmov    q3, q4
    vmov    q4, q5
    vmov    q5, q6

    add r5, r5, #1
    cmp r5, r3
    blt .BCFU8UVNeon_HeiLoop
    sub r0, r0, r8
    sub r1, r1, r9

    add r4, r4, #8
    cmp r4, r2
    blt .BCFU8UVNeon_WidLoop
    vpop {q4-q7}    
    pop {r4-r10}
    bx  lr

//////////////////////////////
////box_mean_uint16_row_uv_neon
//////////////////////////////
////r0:p_src
////r1:p_dst
////r2:width
////r3:height
////r4:row_idx
////r5:col_idx
////r6:src_wjump = (src_wstride - width)<<1
////r7:dst_wjump = (dst_wstride - width)<<1
///////////////////////////////
.globl  _box_mean_uint16_row_uv_neon
_box_mean_uint16_row_uv_neon:
////#0 INITIATE SOME REGISTER
    push    {r4-r7} @reserve the r4-r6
    vpush   {q4-q7} @reserve the q4-q7
////#0 r6=(src_wstride-width)<<1 r7 = (dst_wstride - width)<<1
    ldr r6, [r3, #4]
    ldr r7, [r3, #8]
    ldr r3, [r3, #0]

    sub r6, r6, r2
    sub r7, r7, r2
    lsl r6, r6, #1
    lsl r7, r7, #1
    ////#1 start to col loop
    mov r5, #0
    pld [r0, #128]
    vmov.u32    q13, #2
.BRFU16UVNeon_HeiLoop:
    vld1.16  {q1}, [r0,:128]!
    pld [r0, #128]
    mov r4, #8
    vdup.32  d1, d2[0]
////#1.1 width loo
.BRFU16UVNeon_WidLoop:
////#1.2load d2
    vld1.16 {q2}, [r0,:128]!
    pld [r0,#128]
    cmp r4, r2
    blt .BRFU16UVNW_Next
    vdup.32  d4, d3[1]
    sub r0, r0, #16
.BRFU16UVNW_Next:
    vext.8  q3, q0, q1, #8
    vext.8  q4, q0, q1, #12
    vext.8  q5, q1, q2, #4

    vaddl.u16   q6, d6, d8
    vaddl.u16   q7, d7, d9
    vaddl.u16   q8, d2, d10
    vaddl.u16   q9, d3, d11

    vadd.u32    q6, q6, q8
    vadd.u32    q7, q7, q9

    vadd.u32    q6, q6, q13
    vadd.u32    q7, q7, q13

    vshrn.u32   d12, q6, #2
    vshrn.u32   d13, q7, #2
    vst1.16  {q6}, [r1,:128]!
    vmov    d1, d3
    vmov    q1, q2

    add r4, r4, #8
    cmp r4, r2
    ble .BRFU16UVNeon_WidLoop
    add r0, r0, r6
    add r1, r1, r7
    add r5, r5, #1
    cmp r5, r3
    blt .BRFU16UVNeon_HeiLoop
    vpop {q4-q7}
    pop {r4-r7}
    bx  lr

//////////////////////////////
////box_mean_uint16_col_uv_neon
//////////////////////////////
////r0:p_src
////r1:p_dst
////r2:width
////r3:height
////r4:row_idx
////r5:col_idx
////r6:src_wjump = src_stride << 1
////r7:dst_wjump = dst_stride << 1
////r8:src_hjump = (src_wstride*height-8)<<1
////r9:dst_hjump = (dst_wstride*height-8)<<1
////r10:col_idx_1 = col_idx+1
///////////////////////////////
.globl  _box_mean_uint16_col_uv_neon
_box_mean_uint16_col_uv_neon:
////#0 INITIATE SOME REGISTER
    push    {r4-r10} @reserve the r4-r9
    vpush   {q4-q7} @reserve the q4-q7
////#0 r8=src_hjump, r9 = dst_hjump, r6=src_wjump, r7=dst_wjump
    ldr r6, [r3, #4]
    ldr r7, [r3, #8]
    ldr r3, [r3, #0]

    mul r8, r6, r3
    mul r9, r7, r3
    lsl r6, r6, #1
    lsl r7, r7, #1

    sub r8, r8, #8
    sub r9, r9, #8
    lsl r8, r8, #1
    lsl r9, r9, #1
////#1 start to width loop
    vmov.u32    q11, #2
    mov r4, #0

.BCFU16UVNeon_WidLoop:
    vld1.16  {q5}, [r0,:128]
    add r0, r0, r6
    pld [r0,#128]
    vmov    q3, q5
    vmov    q4, q5
    vaddl.u16   q0, d6, d8
    vaddw.u16   q0, q0, d10

    vaddl.u16   q10, d7, d9
    vaddw.u16   q10, q10, d11

    vmov.u16    q9, #0
    mov r5, #0
    mov r10, #1
////#1.1 height loop
.BCFU16UVNeon_HeiLoop:
////#1.2load q8
    cmp r10, r3
    blt .LUV3
    vmov    q6, q5
    b   .LUV4
.LUV3:
    vld1.16 {q6}, [r0,:128]
    add r0, r0, r6
    pld [r0,#128]    
    add r10, r10, #1
.LUV4:
    vaddw.u16   q0, q0, d12
    vaddw.u16   q10, q10, d13
    vsubw.u16   q0, q0, d18
    vsubw.u16   q10, q10, d19

    vadd.u32    q12, q0, q11
    vadd.u32    q13, q10, q11
    vshrn.u32   d24, q12, #2
    vshrn.u32   d25, q13, #2
    vst1.16  {q12}, [r1,:128]
    add r1, r1, r7

    vmov    q9, q3
    vmov    q3, q4
    vmov    q4, q5
    vmov    q5, q6

    add r5, r5, #1
    cmp r5, r3
    blt .BCFU16UVNeon_HeiLoop
    sub r0, r0, r8
    sub r1, r1, r9

    add r4, r4, #8
    cmp r4, r2
    blt .BCFU16UVNeon_WidLoop
    vpop {q4-q7}
    pop {r4-r10}
    bx  lr

//////////////////////////////
////calc_square_neon
//////////////////////////////
////r0:p_src
////r1:p_dst
////r2:width
////r3:height
////r4:row_idx
////r5:col_idx
////r6:src_wjump = src_wstride - width
////r7:dst_wjump = (dst_wstride - width)<<1
///////////////////////////////
.globl  _calc_square_neon
_calc_square_neon:
////#0 INITIATE SOME REGISTER
    push    {r4-r7} @reserve the r4-r6
////#0 r6=(src_wstride-width)<<1 r7 = (dst_wstride - width)<<1
    ldr r6, [r3, #4]
    ldr r7, [r3, #8]
    ldr r3, [r3, #0]

    sub r6, r6, r2
    sub r7, r7, r2
    lsl r7, r7, #1
    ////#1 start to col loop
    mov r5, #0
.CSQUARE_HeiLoop:
    pld [r0, #128]
    mov r4, #0
    ////#1.1 width loop
.CSQUARE_WidLoop:
    ////#1.2load d2
    vld1.8 {q2}, [r0,:128]!
    pld [r0,#128]
    vmull.u8    q0, d4, d4
    vmull.u8    q1, d5, d5
    vst1.16  {q0,q1}, [r1,:128]!
    add r4, r4, #16
    cmp r4, r2
    blt .CSQUARE_WidLoop
    add r0, r0, r6
    add r1, r1, r7
    add r5, r5, #1
    cmp r5, r3
    blt .CSQUARE_HeiLoop
    pop {r4-r7}
    bx  lr


#define GF_A_SHIFT  6
#define GF_B_SHIFT  4
#define GF_AB_SUM   (GF_A_SHIFT+GF_B_SHIFT)
#define GF_A_SCALE  (1<<GF_A_SHIFT)
#define GF_B_SCALE  (1<<GF_B_SHIFT)
#define GF_B_VALUE  256
//////////////////////////////
////calc_dst_neon(unsigned char **pp_ptr,int width, int height, int stride);
//////////////////////////////
////r0:p_mean_a
////r1:p_mean_b
////r2:p_src
////r3:p_dst
////r4:width
////r5:height
////r6:mean_a_wjump = mean_a_wstride - width
////r7:mean_b_wjump = (mean_b_wstride - width)<<1
////r8:src_wjump = src_wstride - width
////r9:dst_wjump = dst_wstride - width
////r10:row_idx
////r11:col_idx
///////////////////////////////
.globl  _calc_dst_neon
_calc_dst_neon:
////#0 INITIATE SOME REGISTER
    push    {r4-r11} @reserve the r4-r11
    vpush   {q4-q7} @reserve the q4-q7
////#0 r6=mean_a_wjump, r7=mean_b_wjump, r8=src_jump, r9=dst_jump
    ldr	r4, [r3, #4]
    ldr	r5, [r3, #8]
    ldr	r6, [r3, #12]
    ldr	r7, [r3, #16]
    ldr	r8, [r3, #20]
    ldr	r9, [r3, #24]
    ldr	r3, [r3, #0]
    sub r6, r6, r4
    sub r7, r7, r4
    lsl r7, r7, #1
    sub r8, r8, r4
    sub r9, r9, r4
////#1 q0 = (GF_A_SCALE*GF_B_SCALE)/2; q1 = 256
    vmov.u32    q0, #(GF_A_SCALE*GF_B_SCALE)/2
    vmov.u16    q1, #(GF_B_VALUE>>1)
    vshl.u16    q1, #1
////#1 start to col loop
    mov r11, #0
.CALCDST_HeiLoop:
    pld [r0, #128]
    pld [r1, #128]
    pld [r2, #128]
    mov r10, #0
////#1.1 width loop
.CALCDST_WidLoop:
////#1.2load d2
    vld1.8  {q2}, [r0,:128]!
    pld [r0, #128]
    vld1.8  {q3}, [r2,:128]!
    pld [r2, #128]
    vld1.16  {q4,q5}, [r1,:128]!
    pld [r1, #128]
    
    vmull.u8    q6, d4, d6
    vmull.u8    q7, d5, d7

    vshll.u16    q10, d8, #GF_A_SHIFT
    vshll.u16    q11, d9, #GF_A_SHIFT
    vshll.u16    q12, d10, #GF_A_SHIFT
    vshll.u16    q13, d11, #GF_A_SHIFT
    vadd.u32    q10, q10, q0
    vadd.u32    q11, q11, q0
    vadd.u32    q12, q12, q0
    vadd.u32    q13, q13, q0

    vshll.u16    q9, d15, #GF_B_SHIFT
    vshll.u16    q8, d14, #GF_B_SHIFT
    vshll.u16    q7, d13, #GF_B_SHIFT
    vshll.u16    q6, d12, #GF_B_SHIFT

    vadd.u32   q6, q6, q10
    vadd.u32   q7, q7, q11
    vadd.u32   q8, q8, q12
    vadd.u32   q9, q9, q13
    
    vshrn.i32   d12, q6, #GF_AB_SUM
    vshrn.i32   d13, q7, #GF_AB_SUM
    vshrn.i32   d14, q8, #GF_AB_SUM
    vshrn.i32   d15, q9, #GF_AB_SUM

    vqsub.u16   q6, q6, q1
    vqsub.u16   q7, q7, q1
    vqmovn.u16   d12, q6
    vqmovn.u16   d13, q7

    vst1.8  {q6}, [r3,:128]!
    add r10, r10, #16
    cmp r10, r4
    blt .CALCDST_WidLoop
    add r0, r0, r6
    add r1, r1, r7
    add r2, r2, r8
    add r3, r3, r9
    add r11, r11, #1
    cmp r11, r5
    blt .CALCDST_HeiLoop
    vpop    {q4-q7}
    pop {r4-r11}
    bx  lr

//////////////////////////////
////calc_ab_neon(unsigned short *p_mean_II,unsigned char *p_mean_I, unsigned char *p_lookup_a, int *param);
//////////////////////////////
////r0:p_mean_II
////r1:p_mean_I
////r2:p_lookup_a
////r3:p_a
////r4:p_b
////r5:width
////r6:height
////r7:row_idx
////r8:col_idx
////r9--r12:free
///////////////////////////////
.globl  _calc_ab_neon
_calc_ab_neon:
////#0 INITIATE SOME REGISTER
    push    {r4-r12} @reserve the r4-r12
    vpush   {q4-q7} @reserve the q4-q7
    ////#0 load pointer parameter to r4--r6
    //r4 = (mean_II_wstride - width)<<1
    //r5 = mean_I_wstride - width
    //r6 = a_wstride - width
    //r7 = (b_wstride - width)<<1
    ldr r8, [r3, #8]
    ldr	r4, [r3, #16]
    ldr	r5, [r3, #20]
    ldr	r6, [r3, #24]
    ldr	r7, [r3, #28]
    sub r4, r4, r8
    sub r5, r5, r8
    sub r6, r6, r8
    sub r7, r7, r8
    lsl r4, r4, #1
    lsl r7, r7, #1
    vmov	d28, r4, r5
    vmov	d27, r6, r7
    ldr	r4, [r3, #4]
    ldr	r5, [r3, #8]
    ldr	r6, [r3, #12]
    ldr	r3, [r3, #0]
		////#1 q10 = 16384; q9 = #GF_B_VALUE*#GF_A_SCALE q8 = #GF_A_SCALE/2
    vmov.u16    q10, #16384
    vmov.u16    q9, #GF_A_SCALE
    vmov.u32    q8, #GF_A_SCALE/2
    vmov.u16    q7, #(GF_B_VALUE*GF_A_SCALE)
    ////#1 start to col loop
    mov r8, #0
.CALCAB_HeiLoop:
    pld [r0, #128]
    pld [r1, #128]
    mov r7, #0
////#1.1 width loop
.CALCAB_WidLoop:
////#1.2load d2
    vld1.16  {q0}, [r0,:128]!
    pld [r0, #128]
    vld1.8  {d2}, [r1,:64]!
    pld [r1, #128]

    vmull.u8    q2, d2, d2
    vqsub.u16   q2, q0, q2
    vcge.u16    q3, q2, q10
    vbit    q2, q10, q3

    vmov    d30, r5, r6
    vmov    d29, r7, r8

    vmov.u16    r9, d4[0]
    ldrb    r9, [r2,r9]
    vmov.u16    r10, d4[1]
    ldrb    r10, [r2,r10]
    vmov.u16    r11, d4[2]
    ldrb    r11, [r2,r11]
    vmov.u16    r12, d4[3]
    ldrb    r12, [r2,r12]

    vmov.u16    r5, d5[0]
    ldrb    r5, [r2,r5]
    vmov.u16    r6, d5[1]
    ldrb    r6, [r2,r6]
    vmov.u16    r7, d5[2]
    ldrb    r7, [r2,r7]
    vmov.u16    r8, d5[3]
    ldrb    r8, [r2,r8]

    vmov.u8 d3[0], r9
    vmov.u8 d3[1], r10
    vmov.u8 d3[2], r11
    vmov.u8 d3[3], r12
    vmov.u8 d3[4], r5
    vmov.u8 d3[5], r6
    vmov.u8 d3[6], r7
    vmov.u8 d3[7], r8

    vsubw.u8    q3, q9, d3
    vmovl.u8    q4, d2
    vmul.s16    q3, q3, q4
    vst1.8  {d3}, [r3,:64]!
    vmov    r5, r6, d30
    vmov    r7, r8, d29
    vadd.u16    q3, q3, q7
     
    vshll.u16   q5, d6, #GF_B_SHIFT
    vshll.u16   q6, d7, #GF_B_SHIFT
    vadd.u32    q5, q5, q8
    vadd.u32    q6, q6, q8
    vshrn.u32   d10, q5, #GF_A_SHIFT
    vshrn.u32   d11, q6, #GF_A_SHIFT
    vst1.16  {q5}, [r4,:128]!
    add r7, r7, #8
    cmp r7, r5
    blt .CALCAB_WidLoop
    vmov	r9, r10, d28
    vmov	r11, r12, d27
    
    add r0, r0, r9
    add r1, r1, r10
    add r3, r3, r11
    add r4, r4, r12
    
    add r8, r8, #1
    cmp r8, r6
    blt .CALCAB_HeiLoop
    vpop    {q4-q7}
    pop {r4-r12}
    bx  lr

//////////////////////////////
////box_mean_uint8_y_neon(unsigned char *p_src, unsigned char *p_dst,int param1, int param2)
//////////////////////////////
////r0:p_src
////r1:p_dst
////r2:width
////r3:height
////r4:wstride
////r5:row_idx
////r6:col_idx
////r7:width_min = width - 8
////r8:height_min = height - 4
////r9:hstride
///////////////////////////////
.globl  _box_mean_uint8_y_neon
_box_mean_uint8_y_neon:
////#0 INITIATE SOME REGISTER
    push    {r4-r12} @reserve the r4-r12
    vpush   {q4-q7} @reserve the q4-q7
////#1 r2=width r3=height r4=wstride r7 = width - 8 r8 = height - 4 r9=hstride=wstride*height-(r3>>16)*8
    lsr r4, r2, #16
    lsr r9, r3, #16
    lsl r2, r2, #16
    lsr r2, r2, #16
    lsl r3, r3, #16
    lsr r3, r3, #16
    mul r7, r4, r3
    lsl r9, r9, #3
    sub r9, r7, r9
    sub r7, r2, #8
    sub r8, r3, #4
    vmov.u16    q6, #32
////#2 start to col loop
    mov r5, #0
    sub r0, r0, #8
_BMU8Y_WidLoop:
////#2.1 read the #0 row to calc the sum q12
    vld1.8  {d0,d1,d2}, [r0,:64]
    add r0, r0, r4
    pld [r0, #128]
    cmp r5, #0
    bne .CRS_EXIT0
    vdup.8  d0, d1[0]
    b   .CRS_EXIT1
.CRS_EXIT0:
    cmp r5, r7
    blt .CRS_EXIT1
    vdup.8  d2, d1[7]
.CRS_EXIT1:
    vext.8  d3, d0, d1, #4
    vext.8  d4, d0, d1, #5
    vext.8  d5, d0, d1, #6
    vext.8  d6, d0, d1, #7
    vext.8  d7, d1, d2, #1
    vext.8  d8, d1, d2, #2
    vext.8  d9, d1, d2, #3

    vaddl.u8    q1, d1, d3
    vaddl.u8    q2, d4, d5
    vaddl.u8    q3, d6, d7
    vaddl.u8    q4, d8, d9

    vadd.u16    q1, q1, q2
    vadd.u16    q3, q3, q4

    vadd.u16    q12, q1, q3
////#2.2 read the #1 row to calc the sum q13
    vld1.8  {d0,d1,d2}, [r0,:64]
    add r0, r0, r4
    pld [r0, #128]
    cmp r5, #0
    bne .CRS_EXIT2
    vdup.8  d0, d1[0]
    b   .CRS_EXIT3
.CRS_EXIT2:
    cmp r5, r7
    blt .CRS_EXIT3
    vdup.8  d2, d1[7]
.CRS_EXIT3:
    vext.8  d3, d0, d1, #4
    vext.8  d4, d0, d1, #5
    vext.8  d5, d0, d1, #6
    vext.8  d6, d0, d1, #7
    vext.8  d7, d1, d2, #1
    vext.8  d8, d1, d2, #2
    vext.8  d9, d1, d2, #3

    vaddl.u8    q1, d1, d3
    vaddl.u8    q2, d4, d5
    vaddl.u8    q3, d6, d7
    vaddl.u8    q4, d8, d9

    vadd.u16    q1, q1, q2
    vadd.u16    q3, q3, q4

    vadd.u16    q13, q1, q3
////#2.3 read the #2 row to calc the sum q14
    vld1.8  {d0,d1,d2}, [r0,:64]
    add r0, r0, r4
    pld [r0, #128]
    cmp r5, #0
    bne .CRS_EXIT4
    vdup.8  d0, d1[0]
    b   .CRS_EXIT5
.CRS_EXIT4:
    cmp r5, r7
    blt .CRS_EXIT5
    vdup.8  d2, d1[7]
.CRS_EXIT5:
    vext.8  d3, d0, d1, #4
    vext.8  d4, d0, d1, #5
    vext.8  d5, d0, d1, #6
    vext.8  d6, d0, d1, #7
    vext.8  d7, d1, d2, #1
    vext.8  d8, d1, d2, #2
    vext.8  d9, d1, d2, #3

    vaddl.u8    q1, d1, d3
    vaddl.u8    q2, d4, d5
    vaddl.u8    q3, d6, d7
    vaddl.u8    q4, d8, d9

    vadd.u16    q1, q1, q2
    vadd.u16    q3, q3, q4

    vadd.u16    q14, q1, q3
////2.4 q7 = q12*5 + q13 + q14
    vshl.u16    q7, q12, #2
    vadd.u16    q7, q7, q12
    vadd.u16    q7, q7, q13
    vadd.u16    q7, q7, q14
    vmov    q8, q12
    vmov    q9, q12
    vmov    q10, q12
    vmov    q11, q12

    mov r6, #0
////#2.5 col loop
_BMU8Y_HeiLoop: 
////#2.6 calc new col
    cmp r6, r8
    ble .L12
    vmov    q15, q14
    b   .L22
.L12:
////#2.7 read the new row to calc the sum q15
    vld1.8  {d0,d1,d2}, [r0,:64]
    add r0, r0, r4
    pld [r0, #128]
    cmp r5, #0
    bne .CRS_EXIT6
    vdup.8  d0, d1[0]
    b   .CRS_EXIT7
.CRS_EXIT6:
    cmp r5, r7
    blt .CRS_EXIT7
    vdup.8  d2, d1[7]
.CRS_EXIT7:
    vext.8  d3, d0, d1, #4
    vext.8  d4, d0, d1, #5
    vext.8  d5, d0, d1, #6
    vext.8  d6, d0, d1, #7
    vext.8  d7, d1, d2, #1
    vext.8  d8, d1, d2, #2
    vext.8  d9, d1, d2, #3

    vaddl.u8    q1, d1, d3
    vaddl.u8    q2, d4, d5
    vaddl.u8    q3, d6, d7
    vaddl.u8    q4, d8, d9

    vadd.u16    q1, q1, q2
    vadd.u16    q3, q3, q4

    vadd.u16    q15, q1, q3
.L22:
    vadd.u16    q7, q7, q15
    vadd.u16    q5, q6, q7
    vshrn.u16   d10, q5, #6
    vst1.8  {d10}, [r1,:64]
    add r1, r1, r4

    vsub.u16 q7, q7, q8
    vmov    q8, q9
    vmov    q9, q10
    vmov    q10, q11
    vmov    q11, q12
    vmov    q12, q13
    vmov    q13, q14
    vmov    q14, q15

    add r6, r6, #1
    cmp r6, r3
    blt _BMU8Y_HeiLoop
    sub r0, r0, r9
    sub r1, r1, r9
    add r5, r5, #8
    cmp r5, r2
    blt _BMU8Y_WidLoop
    mov	r0, #0
    vpop {q4-q7}
    pop {r4-r12}
    bx  lr

//////////////////////////////
////box_mean_uint16_y_neon(unsigned short *p_src, unsigned short *p_dst,int param1, int param2)
//////////////////////////////
////r0:p_src
////r1:p_dst
////r2:width
////r3:height
////r4:wstride
////r5:row_idx
////r6:col_idx
////r7:width_min = width - 8
////r8:height_min = height - 4
////r9:hstride
///////////////////////////////
.globl  _box_mean_uint16_y_neon
_box_mean_uint16_y_neon:
////#0 INITIATE SOME REGISTER
    push    {r4-r12} @reserve the r4-r12
    vpush   {q4-q7} @reserve the q4-q7
////#1 r2=width r3=height r4=wstride*2 r7 = width - 8 r8 = height - 4 r9=hstride=(wstride*height-(r3>>16)*8)*2
    lsr r4, r2, #16
    lsr r9, r3, #16
    lsl r2, r2, #16
    lsr r2, r2, #16
    lsl r3, r3, #16
    lsr r3, r3, #16
    mul r7, r4, r3
    lsl r9, r9, #3
    sub r9, r7, r9
    lsl r9, r9, #1
    lsl r4, r4, #1
    sub r7, r2, #8
    sub r8, r3, #4
    vmov.u32    q2, #4
////#2 start to col loop
    mov r5, #0
    sub r0, r0, #8
_BMU16Y_WidLoop:
////#2.1 read the #0 row to calc the sum q12
    vld1.16  {d0,d1,d2,d3}, [r0,:64]
    add r0, r0, r4
    pld [r0, #128]
    cmp r5, #0
    bne .CRS_EXIT8
    vdup.16  d0, d1[0]
    b   .CRS_EXIT9
.CRS_EXIT8:
    cmp r5, r7
    blt .CRS_EXIT9
    vdup.16  d3, d2[3]
.CRS_EXIT9:
    vext.8  q11, q0, q1, #2
    vext.8  q10, q0, q1, #4
    vext.8  q9, q0, q1, #6
    vext.8  q8, q0, q1, #8
    vext.8  q7, q0, q1, #10
    vext.8  q6, q0, q1, #12
    vext.8  q5, q0, q1, #14

    vaddl.u16   q3, d22, d20
    vaddl.u16   q4, d23, d21    
    vaddw.u16   q3, q3, d18
    vaddw.u16   q4, q4, d19
    vaddw.u16   q3, q3, d16
    vaddw.u16   q4, q4, d17
    vaddw.u16   q3, q3, d14
    vaddw.u16   q4, q4, d15
    vaddw.u16   q3, q3, d12
    vaddw.u16   q4, q4, d13
    vaddw.u16   q3, q3, d10
    vaddw.u16   q4, q4, d11
    vaddw.u16   q3, q3, d0
    vaddw.u16   q4, q4, d1

    vadd.u32    q3, q3, q2
    vadd.u32    q4, q4, q2
    vshrn.u32   d24, q3, #3
    vshrn.u32   d25, q4, #3
////#2.2 read the #1 row to calc the sum q13
    vld1.16  {d0,d1,d2,d3}, [r0,:64]
    add r0, r0, r4
    pld [r0, #128]
    cmp r5, #0
    bne .CRS_EXIT10
    vdup.16  d0, d1[0]
    b   .CRS_EXIT11
.CRS_EXIT10:
    cmp r5, r7
    blt .CRS_EXIT11
    vdup.16  d3, d2[3]
.CRS_EXIT11:
    vext.8  q11, q0, q1, #2
    vext.8  q10, q0, q1, #4
    vext.8  q9, q0, q1, #6
    vext.8  q8, q0, q1, #8
    vext.8  q7, q0, q1, #10
    vext.8  q6, q0, q1, #12
    vext.8  q5, q0, q1, #14

    vaddl.u16   q3, d22, d20
    vaddl.u16   q4, d23, d21    
    vaddw.u16   q3, q3, d18
    vaddw.u16   q4, q4, d19
    vaddw.u16   q3, q3, d16
    vaddw.u16   q4, q4, d17
    vaddw.u16   q3, q3, d14
    vaddw.u16   q4, q4, d15
    vaddw.u16   q3, q3, d12
    vaddw.u16   q4, q4, d13
    vaddw.u16   q3, q3, d10
    vaddw.u16   q4, q4, d11
    vaddw.u16   q3, q3, d0
    vaddw.u16   q4, q4, d1

    vadd.u32    q3, q3, q2
    vadd.u32    q4, q4, q2
    vshrn.u32   d26, q3, #3
    vshrn.u32   d27, q4, #3
////#2.3 read the #2 row to calc the sum q14
    vld1.16  {d0,d1,d2,d3}, [r0,:64]
    add r0, r0, r4
    pld [r0, #128]
    cmp r5, #0
    bne .CRS_EXIT12
    vdup.16  d0, d1[0]
    b   .CRS_EXIT13
.CRS_EXIT12:
    cmp r5, r7
    blt .CRS_EXIT13
    vdup.16  d3, d2[3]
.CRS_EXIT13:
    vext.8  q11, q0, q1, #2
    vext.8  q10, q0, q1, #4
    vext.8  q9, q0, q1, #6
    vext.8  q8, q0, q1, #8
    vext.8  q7, q0, q1, #10
    vext.8  q6, q0, q1, #12
    vext.8  q5, q0, q1, #14

    vaddl.u16   q3, d22, d20
    vaddl.u16   q4, d23, d21    
    vaddw.u16   q3, q3, d18
    vaddw.u16   q4, q4, d19
    vaddw.u16   q3, q3, d16
    vaddw.u16   q4, q4, d17
    vaddw.u16   q3, q3, d14
    vaddw.u16   q4, q4, d15
    vaddw.u16   q3, q3, d12
    vaddw.u16   q4, q4, d13
    vaddw.u16   q3, q3, d10
    vaddw.u16   q4, q4, d11
    vaddw.u16   q3, q3, d0
    vaddw.u16   q4, q4, d1

    vadd.u32    q3, q3, q2
    vadd.u32    q4, q4, q2
    vshrn.u32   d28, q3, #3
    vshrn.u32   d29, q4, #3

////2.4 q5 q7 = q12*5 + q13 + q14
    vshll.u16    q6, d24, #2
    vshll.u16    q7, d25, #2

    vaddw.u16    q6, q6, d24
    vaddw.u16    q7, q7, d25
    vaddw.u16    q6, q6, d26
    vaddw.u16    q7, q7, d27
    vaddw.u16    q6, q6, d28
    vaddw.u16    q7, q7, d29

    vmov    q8, q12
    vmov    q9, q12
    vmov    q10, q12
    vmov    q11, q12

    mov r6, #0
////#2.5 col loop
_BMU16Y_HeiLoop: 
////#2.6 calc new col
    cmp r6, r8
    ble .L32
    vmov    q15, q14
    b   .L42
.L32:
////#2.7 read the new row to calc the sum q15
    vld1.16  {d0,d1,d2,d3}, [r0,:64]
    add r0, r0, r4
    pld [r0, #128]
    cmp r5, #0
    bne .CRS_EXIT14
    vdup.16  d0, d1[0]
    b   .CRS_EXIT15
.CRS_EXIT14:
    cmp r5, r7
    blt .CRS_EXIT15
    vdup.16  d3, d2[3]
.CRS_EXIT15:
    vext.8  q3, q0, q1, #2
    vaddl.u16   q4, d6, d0
    vaddl.u16   q5, d7, d1

    vext.8  q3, q0, q1, #4
    vaddw.u16   q4, q4, d6
    vaddw.u16   q5, q5, d7

    vext.8  q3, q0, q1, #6
    vaddw.u16   q4, q4, d6
    vaddw.u16   q5, q5, d7

    vext.8  q3, q0, q1, #8
    vaddw.u16   q4, q4, d6
    vaddw.u16   q5, q5, d7

    vext.8  q3, q0, q1, #10
    vaddw.u16   q4, q4, d6
    vaddw.u16   q5, q5, d7

    vext.8  q3, q0, q1, #12
    vaddw.u16   q4, q4, d6
    vaddw.u16   q5, q5, d7

    vext.8  q3, q0, q1, #14
    vaddw.u16   q4, q4, d6
    vaddw.u16   q5, q5, d7
    
    vadd.u32    q4, q4, q2
    vadd.u32    q5, q5, q2
    vshrn.u32   d30, q4, #3
    vshrn.u32   d31, q5, #3
.L42:
    vaddw.u16    q6, q6, d30
    vaddw.u16    q7, q7, d31
    vadd.u32    q4, q6, q2
    vadd.u32    q5, q7, q2
    vshrn.u32   d8, q4, #3
    vshrn.u32   d9, q5, #3
    vst1.16  {q4}, [r1,:64]
    add r1, r1, r4

    vsubw.u16 q6, q6, d16
    vsubw.u16 q7, q7, d17

    vmov    q8, q9
    vmov    q9, q10
    vmov    q10, q11
    vmov    q11, q12
    vmov    q12, q13
    vmov    q13, q14
    vmov    q14, q15

    add r6, r6, #1
    cmp r6, r3
    blt _BMU16Y_HeiLoop
    sub r0, r0, r9
    sub r1, r1, r9
    add r5, r5, #8
    cmp r5, r2
    blt _BMU16Y_WidLoop
    mov	r0, #0
    vpop {q4-q7}
    pop {r4-r12}
    bx  lr

//////////////////////////////
////box_mean_uint8_uv_neon(unsigned char *p_src, unsigned char *p_dst,int param1, int param2)
//////////////////////////////
////r0:p_src
////r1:p_dst
////r2:width
////r3:height
////r4:wstride
////r5:row_idx
////r6:col_idx
////r7:width_min = width - 8
////r8:height_min = height - 2
////r9:hstride
///////////////////////////////
.globl  _box_mean_uint8_uv_neon
_box_mean_uint8_uv_neon:
////#0 INITIATE SOME REGISTER
    push    {r4-r12} @reserve the r4-r12
    vpush   {q4-q7} @reserve the q4-q7
////#1 r2=width r3=height r4=wstride r7 = width - 8 r8 = height - 2 r9=hstride=wstride*height-(r3>>16)*8
    lsr r4, r2, #16
    lsr r9, r3, #16
    lsl r2, r2, #16
    lsr r2, r2, #16
    lsl r3, r3, #16
    lsr r3, r3, #16
    mul r7, r4, r3
    lsl r9, r9, #3
    sub r9, r7, r9
    sub r7, r2, #8
    sub r8, r3, #2
    vmov.u16    q6, #8
////#2 start to col loop
    mov r5, #0
    sub r0, r0, #8
_BMU8UV_WidLoop:
////#2.1 read the #0 row to calc the sum q12
    vld1.8  {d0,d1,d2}, [r0,:64]
    add r0, r0, r4
    pld [r0, #128]
    cmp r5, #0
    bne .CRS_EXIT16
    vdup.16  d0, d1[0]
    b   .CRS_EXIT17
.CRS_EXIT16:
    cmp r5, r7
    blt .CRS_EXIT17
    vdup.16  d2, d1[3]
.CRS_EXIT17:
    vext.8  d3, d0, d1, #4
    vext.8  d4, d0, d1, #6
    vext.8  d5, d1, d2, #2

    vaddl.u8    q1, d1, d3
    vaddl.u8    q2, d4, d5
    
    vadd.u16    q12, q1, q2
////2.2 q7 = q12*3
    vshl.u16    q7, q12, #1
    vadd.u16    q7, q7, q12
    vmov    q10, q12
    vmov    q11, q12

    mov r6, #0
////#2.3 col loop
_BMU8UV_HeiLoop: 
////#2.4 calc new col
    cmp r6, r8
    ble .L52
    vmov    q13, q12
    b   .L62
.L52:
////#2.5 read the new row to calc the sum q13
    vld1.8  {d0,d1,d2}, [r0,:64]
    add r0, r0, r4
    pld [r0, #128]
    cmp r5, #0
    bne .CRS_EXIT18
    vdup.16  d0, d1[0]
    b   .CRS_EXIT19
.CRS_EXIT18:
    cmp r5, r7
    blt .CRS_EXIT19
    vdup.16  d2, d1[3]
.CRS_EXIT19:
    vext.8  d3, d0, d1, #4
    vext.8  d4, d0, d1, #6
    vext.8  d5, d1, d2, #2

    vaddl.u8    q1, d1, d3
    vaddl.u8    q2, d4, d5

    vadd.u16    q13, q1, q2
.L62:
    vadd.u16    q7, q7, q13
    vadd.u16    q5, q6, q7
    vshrn.u16   d10, q5, #4
    vst1.8  {d10}, [r1,:64]
    add r1, r1, r4

    vsub.u16 q7, q7, q10
    vmov    q10, q11
    vmov    q11, q12
    vmov    q12, q13

    add r6, r6, #1
    cmp r6, r3
    blt _BMU8UV_HeiLoop
    sub r0, r0, r9
    sub r1, r1, r9
    add r5, r5, #8
    cmp r5, r2
    blt _BMU8UV_WidLoop
    mov	r0, #0
    vpop {q4-q7}
    pop {r4-r12}
    bx  lr

//////////////////////////////
////box_mean_uint16_uv_neon(unsigned short *p_src, unsigned short *p_dst,int param1, int param2)
//////////////////////////////
////r0:p_src
////r1:p_dst
////r2:width
////r3:height
////r4:wstride
////r5:row_idx
////r6:col_idx
////r7:width_min = width - 8
////r8:height_min = height - 2
////r9:hstride
///////////////////////////////
.globl  _box_mean_uint16_uv_neon
_box_mean_uint16_uv_neon:
////#0 INITIATE SOME REGISTER
    push    {r4-r12} @reserve the r4-r12
    vpush   {q4-q7} @reserve the q4-q7
////#1 r2=width r3=height r4=wstride*2 r7 = width - 8 r8 = height - 2 r9=hstride=(wstride*height-(r3>>16)*8)*2
    lsr r4, r2, #16
    lsr r9, r3, #16
    lsl r2, r2, #16
    lsr r2, r2, #16
    lsl r3, r3, #16
    lsr r3, r3, #16
    mul r7, r4, r3
    lsl r9, r9, #3
    sub r9, r7, r9
    lsl r9, r9, #1
    lsl r4, r4, #1
    sub r7, r2, #8
    sub r8, r3, #2
    vmov.u32    q8, #2
////#2 start to col loop
    mov r5, #0
    sub r0, r0, #8
_BMU16UV_WidLoop:
////#2.1 read the #0 row to calc the sum q12
    vld1.16  {d0,d1,d2,d3}, [r0,:64]
    add r0, r0, r4
    pld [r0, #128]
    cmp r5, #0
    bne .CRS_EXIT20
    vdup.32  d0, d1[0]
    b   .CRS_EXIT21
.CRS_EXIT20:
    cmp r5, r7
    blt .CRS_EXIT21
    vdup.32  d3, d2[1]
.CRS_EXIT21:
    vext.8  q2, q0, q1, #4
    vext.8  q3, q0, q1, #12

    vaddl.u16   q4, d0, d4
    vaddl.u16   q5, d1, d5
    vaddl.u16   q6, d1, d6
    vaddl.u16   q7, d2, d7

    vadd.u32    q4, q4, q6
    vadd.u32    q5, q5, q7
    vadd.u32    q4, q4, q8
    vadd.u32    q5, q5, q8

    vshrn.u32   d24, q4, #2
    vshrn.u32   d25, q5, #2

////2.2 q14 q15 = q12*3
    vshll.u16    q14, d24, #1
    vshll.u16    q15, d25, #1
    vaddw.u16    q14, q14, d24
    vaddw.u16    q15, q15, d25

    vmov    q10, q12
    vmov    q11, q12

    mov r6, #0
////#2.3 col loop
_BMU16UV_HeiLoop: 
////#2.4 calc new col
    cmp r6, r8
    ble .L72
    vmov    q13, q12
    b   .L82
.L72:
////#2.5 read the new row to calc the sum q13
    vld1.16  {d0,d1,d2,d3}, [r0,:64]
    add r0, r0, r4
    pld [r0, #128]
    cmp r5, #0
    bne .CRS_EXIT22
    vdup.32  d0, d1[0]
    b   .CRS_EXIT23
.CRS_EXIT22:
    cmp r5, r7
    blt .CRS_EXIT23
    vdup.32  d3, d2[1]
.CRS_EXIT23:
    vext.8  q2, q0, q1, #4
    vext.8  q3, q0, q1, #12

    vaddl.u16   q4, d0, d4
    vaddl.u16   q5, d1, d5
    vaddl.u16   q6, d1, d6
    vaddl.u16   q7, d2, d7

    vadd.u32    q4, q4, q6
    vadd.u32    q5, q5, q7
    vadd.u32    q4, q4, q8
    vadd.u32    q5, q5, q8

    vshrn.u32   d26, q4, #2
    vshrn.u32   d27, q5, #2

.L82:
    vaddw.u16    q14, q14, d26
    vaddw.u16    q15, q15, d27
    vadd.u32    q4, q14, q8
    vadd.u32    q5, q15, q8
    vshrn.u32   d8, q4, #2
    vshrn.u32   d9, q5, #2
    vst1.16  {q4}, [r1,:64]
    add r1, r1, r4

    vsubw.u16 q14, q14, d20
    vsubw.u16 q15, q15, d21

    vmov    q10, q11
    vmov    q11, q12
    vmov    q12, q13

    add r6, r6, #1
    cmp r6, r3
    blt _BMU16UV_HeiLoop
    sub r0, r0, r9
    sub r1, r1, r9
    add r5, r5, #8
    cmp r5, r2
    blt _BMU16UV_WidLoop
    mov	r0, #0
    vpop {q4-q7}
    pop {r4-r12}
    bx  lr

//////////////////////////////
////ZXCLAHEInterploateEntry_neon(int *pHistLU, int *pHistLB, int *pHistRU, int *param2)
//////////////////////////////
////d31:pHistLU|pHistLB
////d30:pHistLB|pHistRB
////d29:p_src|p_dst
////d28:src_wjump|dst_wjump
////d27:col_idx|height
////d26:row_idx|width
////q12:nDivScalar
////q11:width-1|width-2|width-3|width-4
////q10:4|4|4|4
////q9:0|1|2|3
////q8:y_conv
////q7:y
////q6:x_conv
////q5:x
///////////////////////////////
.globl  _ZXCLAHEInterploateEntry_neon
_ZXCLAHEInterploateEntry_neon:
    ////#0 INITIATE SOME REGISTER
    push    {r4-r12} @reserve the r4-r12
    vpush   {q4-q7} @reserve the q4-q7

    ldr r4, [r3, #4]    //p_src
    ldr r5, [r3, #8]    //p_dst
    ldr r6, [r3, #12]   //width
    ldr r7, [r3, #16]   //height
    ldr r8, [r3, #20]   //src_stride
    ldr r9, [r3, #24]   //dst_stride
    ldr r10, [r3, #28]  //nDivScalar
    ldr r11, [r3, #32]  //nDivShift
    ldr r3, [r3, #0]    //RB
    ////////LU|LB,RU|RB,p_src|p_dst/////////
    vmov    d31, r0, r1
    vmov    d30, r2, r3
    vmov    d29, r4, r5
    ///////////q12 = nDivScalar
    vdup.32 q12, r10
    /////////q10=4,q9={0,1,2,3}
    vmov.u32 q10, #4
    mov r0, #0
    mov r1, #1
    mov r2, #2
    mov r3, #3
    vmov    d18, r0, r1
    vmov    d19, r2, r3
    ///////////q13 = col_idx|height|row_idx|width
    mov r0, #0
    vmov    d27, r0, r7
    vmov    d26, r0, r6
    //////////d28=src_jump|dst_jump,q11={width-1,width-2,width-3,width-4}
    sub r1, r9, r6
    vmov    d28, r8, r1
    sub r0, r6, #1
    sub r1, r6, #2
    sub r2, r6, #3
    sub r3, r6, #4
    vmov    d22, r0, r1
    vmov    d23, r2, r3
    //////////q8=y_conv,q7=y
    sub r0, r7, #1
    vdup.32 q8, r0
    vmov.u32 q7, #0
.ZXCLAHEIplneonHei:
    vmov    q6, q11
    vmov    q5, q9
.ZXCLAHEIplneonWid:
    vmov    r0, r1, d29
    vmov    r2, r3, d26
    //#0 read 4 nGrays
    ldr r1, [r0, r2]
    and r0, r1, #0xFF
    mov r2, r1, lsr #16
    and r2, r2, #0xFF
    mov r3, r1, lsr #24
    and r3, r3, #0xFF
    mov r1, r1, lsr #8
    and r1, r1, #0xFF

    //#1 read 4 pHistLU|pHistLB
    vmov    r7, r11, d31

    ldr r4, [r7, r0, lsl #2]
    ldr r5, [r7, r1, lsl #2]
    ldr r6, [r7, r2, lsl #2]
    ldr r7, [r7, r3, lsl #2]

    ldr r8, [r11, r0, lsl #2]
    ldr r9, [r11, r1, lsl #2]
    ldr r10, [r11, r2, lsl #2]
    ldr r11, [r11, r3, lsl #2]
    
    vmov    d0, r4, r5
    vmov    d1, r6, r7
    vmov    d4, r8, r9
    vmov    d5, r10, r11

    //#2 read 4 pHistRU|pHistRB
    vmov    r7, r11, d30
    ldr r4, [r7, r0, lsl #2]
    ldr r5, [r7, r1, lsl #2]
    ldr r6, [r7, r2, lsl #2]
    ldr r7, [r7, r3, lsl #2]

    ldr r8, [r11, r0, lsl #2]
    ldr r9, [r11, r1, lsl #2]
    ldr r10, [r11, r2, lsl #2]
    ldr r11, [r11, r3, lsl #2]

    vmov    d2, r4, r5
    vmov    d3, r6, r7
    vmov    d6, r8, r9
    vmov    d7, r10, r11

    //#3 multiply operation
    vmul.u32    q0, q0, q6
    vmul.u32    q1, q1, q5
    vmul.u32    q2, q2, q6
    vmul.u32    q3, q3, q5
    
    vadd.u32   q0, q0, q1
    vadd.u32   q2, q2, q3
    
    vmul.u32    q0, q0, q8
    vmul.u32    q2, q2, q7

    vadd.u32    q0, q0, q2
    vmul.u32    q0, q0, q12
    vshrn.u32   d0, q0, #16
    vshrn.u16   d0, q0, #8
    vmov    r0, r1, d29
    vst1.32  {d0[0]}, [r1,:32]!
    vmov    d29, r0, r1

    vadd.u32    q5, q5, q10
    vsub.u32    q6, q6, q10
    vmov    r0, r1, d26
    add r0, r0, #4
    vmov    d26, r0, r1
    cmp r0, r1
    blt .ZXCLAHEIplneonWid
    //reset row_idx & width
    vmov    r0, r1, d26
    mov r0, #0
    vmov    d26, r0, r1
    ///////////reset y_conv,y
    vmov.u32    q0, #1
    vsub.u32    q8, q8, q0
    vadd.u32    q7, q7, q0
    ///////////src jump, dst jump///////
    vmov    r0, r1, d29
    vmov    r2, r3, d28
    add r0, r0, r2
    add r1, r1, r3
    vmov    d29, r0, r1
    //////////col_idx, height///////////
    vmov    r4, r5, d27
    add r4, r4, #1
    vmov    d27, r4, r5
    cmp r4, r5
    blt .ZXCLAHEIplneonHei

    vpop {q4-q7}
    pop {r4-r12}
    bx  lr

