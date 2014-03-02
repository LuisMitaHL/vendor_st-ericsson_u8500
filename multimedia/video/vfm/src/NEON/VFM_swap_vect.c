/*void amit(){
__asm__(
"@.section .text"

	".global swap_vector_64"
	".type swap_vector_64, %function"

	".global swap_vector_128"
	".type swap_vector_128, %function"

	".global swap_vector_256"
	".type swap_vector_256, %function"

	".global swap_vector_256_lu"
	".type swap_vector_256_lu, %function"

	".fpu neon"
	".align 4"
}*/
void swap_vector_64(unsigned long long *dst, unsigned long long *src, int noe){
__asm__(
	"pld	[r1, #(32 * 0)];"
	"pld	[r1, #(32 * 1)];"
	"vld1.64  {d0}, [r1,:64]!;"
	"vrev64.8 d1, d0;"
	"vst1.64  {d1}, [r0,:64]!;"
	"subs r2, r2, #1;"
	"bne swap_vector_64;"
	"bx lr;"
);
}

void swap_vector_128(unsigned long long *dst, unsigned long long *src, int noe){
__asm__(
	"pld	[r1, #(32 * 0)];"
	"pld	[r1, #(32 * 1)];"
	"pld	[r1, #(32 * 2)];"
	"pld	[r1, #(32 * 3)];"
	"pld	[r1, #(32 * 4)];"
	"pld	[r1, #(32 * 5)];"
	"pld	[r1, #(32 * 6)];"
	"pld	[r1, #(32 * 7)];"
	"vld1.64  {q0}, [r1,:64]!;"
	"vrev64.8 q0, q0;"
	"vst1.64  {q0}, [r0,:64]!;"
	"subs r2, r2, #1;"
	"bne swap_vector_128;"
	"bx lr;"
);
}

void swap_vector_256(unsigned long long *dst, unsigned long long *src, int noe){
__asm__(
	"pld	[r1, #(32 * 0)];"
	"pld	[r1, #(32 * 1)];"
	"pld	[r1, #(32 * 2)];"
	"pld	[r1, #(32 * 3)];"
	"pld	[r1, #(32 * 4)];"
	"pld	[r1, #(32 * 5)];"
	"pld	[r1, #(32 * 6)];"
	"pld	[r1, #(32 * 7)];"
	"vld1.64  {d0,d1,d2,d3}, [r1,:64]!;"
	"vrev64.8 q0, q0;"
	"vrev64.8 q1, q1;"
	"vst1.64  {d0,d1,d2,d3}, [r0,:64]!;"
	"subs r2, r2, #1;"
	"bne swap_vector_256;"
	"bx lr;"
);
}
void swap_vector_256_lu(unsigned long long *dst, unsigned long long *src, int noe){
__asm__(
	"pld	[r1, #(32 * 0)];"
	"pld	[r1, #(32 * 1)];"
	"pld	[r1, #(32 * 2)];"
	"pld	[r1, #(32 * 3)];"
	"pld	[r1, #(32 * 4)];"
	"pld	[r1, #(32 * 5)];"
	"pld	[r1, #(32 * 6)];"
	"pld	[r1, #(32 * 7)];"

	"vld1.64  {d0 - d3}, [r1,:64]!;"
	"vrev64.8 q0, q0;"
	"vrev64.8 q1, q1;"
	"vst1.64  {d0 - d3}, [r0,:64]!;"

	"vld1.64  {d4 - d7}, [r1,:64]!;" 	
	"vrev64.8 q2, q2;"     			
	"vrev64.8 q3, q3;"     			
	"vst1.64  {d4 - d7}, [r0,:64]!;" 	

	"vld1.64  {d8 - d11}, [r1,:64]!;" 	
	"vrev64.8 q4, q4;"     			
	"vrev64.8 q5, q5;"     			
	"vst1.64  {d8 - d11}, [r0,:64]!;"

	"vld1.64  {d12 - d15}, [r1,:64]!;" 	
	"vrev64.8 q6, q6;"     			
	"vrev64.8 q7, q7;"     			
	"vst1.64  {d12 - d15}, [r0,:64]!;"

	"subs r2, r2, #1;"				
	"bne swap_vector_256_lu;"
	"bx lr;"
);
}

