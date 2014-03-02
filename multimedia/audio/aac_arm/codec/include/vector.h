/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _vector_h_
#define _vector_h_

#include "audiolibs_common.h"

#ifndef _NMF_MPC_

extern CPPCALL void vector_set_value(Float *p,int len,Float value,int incr);
extern CPPCALL void vector_window(Float *p,Float WINDOW_MEM *win,int len,
			  int incr1,int incr2);
extern CPPCALL void vector_window_copy(Float *p, Float *q, Float WINDOW_MEM *win, int len,
			       int incr1, int incr2, int incr3);
extern CPPCALL void vector_window_add(Float *p, Float *q, Float WINDOW_MEM *win, int len,
			      int incr1, int incr2, int incr3);
extern CPPCALL void vector_window_sub(Float *p, Float *q, Float WINDOW_MEM *win, int len,
			      int incr1, int incr2, int incr3);

extern CPPCALL void  vector_copy_2y(Float YMEM *p,Float const *q,int len,int incr1,int incr2);
extern CPPCALL void  vector_copy_neg(Float *p,Float *q,int len,int incr1,int incr2);
extern CPPCALL void  vector_swap(Float *p,Float *q,int len,int incr1,int incr2);
extern CPPCALL void  vector_add(Float *p,Float *q,int len,int incr1, int incr2);
extern CPPCALL void  vector_sub(Float *p, Float *q, int len, int incr1, int incr2);
extern CPPCALL void  vector_add_copy(Float *res,Float *p,Float *q,int len,int incr);
;
extern CPPCALL void  vector_scale_in_place(Float *p, Float scale, int len, int incr);

extern CPPCALL void  vector_abs(Float *p,Float *q,int len,int incr1);
extern CPPCALL Float vector_max_magnitude(Float *src1, int len, int incr);
extern CPPCALL void  vector_scale_copy_to_buffers(Float *data_ptr,Float **result_ptr,
				  int block_len,Float scale);
extern CPPCALL void  vector_scale_copy_to_buffers_interleaved(Float *data_ptr,Float *result_ptr,
				  int block_len,Float scale);
extern CPPCALL void  vector_scale_add(Float **data_ptr,Float *result_ptr,int block_len,
				  Float scale1,Float scale2);
extern CPPCALL void  vector_scale_add_interleaved(Float *data_ptr,Float *result_ptr,int block_len,
				  Float scale1,Float scale2);
extern CPPCALL void  vector_scale_mix(Float **data_ptr,Float **result_ptr,
							  int block_len, Float *scale);
extern CPPCALL void  vector_scale_mix_interleaved(Float *data_ptr,Float *result_ptr,
							  int block_len, Float *scale);

extern CPPCALL void  vector_stereo_interleave_pan(Float *left, Float *scratch, int len, int nb_chan);
extern CPPCALL void  vector_stereo_interleave_pan_fix(int *left, int *scratch, int len, int nb_chan);

extern CPPCALL void  vector_ms_scale(Float *m,Float *s,Float scale, int len);

extern CPPCALL void  vector_ms(Float *m,Float *s,int len);
extern CPPCALL void  vector_scale(Float *src,Float *dest,Float scale,int len);
extern CPPCALL void  vector_copy(Float *p,Float *q,int len,int incr1,int incr2);
extern CPPCALL void   buffer_reset(char *p, char val, int size);

#ifdef ARM
extern CPPCALL  void byte_swap(int *dest, int* src, int size);
extern CPPCALL void  vector_add_arm(int *p,int *q,int len,int incr1, int incr2);
extern CPPCALL void  vector_sub_arm(int *p, int *q, int len, int incr1, int incr2);
extern CPPCALL void  vector_add_copy_arm(int *res,int *p,int *q,int len,int incr);
extern CPPCALL void vector_window_arm(int *p,int WINDOW_MEM *win,int len,
			  int incr1,int incr2);
extern CPPCALL void vector_window_copy_arm(int *p, int *q, int WINDOW_MEM *win, int len,
			       int incr1, int incr2, int incr3);
extern CPPCALL void vector_window_add_arm(int *p, int *q, int WINDOW_MEM *win, int len,
			      int incr1, int incr2, int incr3);
extern CPPCALL void vector_window_sub_arm(int *p, int *q, int WINDOW_MEM *win, int len,
			      int incr1, int incr2, int incr3);
extern CPPCALL void vector_scale_in_place_arm(int *src, int scale, int len, int incr);
extern CPPCALL void  vector_ms_simd(int *m,int *s,int len);
extern CPPCALL void  vector_copy_arm(int *p, int *q, int len, int incr1, int incr2);
extern CPPCALL void  vector_copy_arm_simd(int * __restrict p,int *__restrict q,int len);
extern CPPCALL AUDIO_API_IMPORT void  buffer_reset_arm(char *p, char val, int size);
extern CPPCALL void  vector_set_value_arm(int *p, int len, int value, int incr);
extern CPPCALL AUDIO_API_IMPORT void  buffer_copy_arm(char *p, char *q, int size);
#ifdef NEON
extern CPPCALL void vector_ms_neon(int *m, int * s, int len);
#elif (defined VECT )
extern CPPCALL void vector_ms_vect(int *m, int * s, int len);
#else // NEON || VECT
extern CPPCALL void vector_ms_arm(int *m, int * s, int len);
#endif /* NEON */
#endif /* ARM */

extern CPPCALL void   buffer_copy(char *p, char *q, int size);
extern CPPCALL void   buffer_init_calloc(char *adr,int size_mem);
extern CPPCALL void  *buffer_calloc(int num,int size);
extern CPPCALL void   buffer_init_calloc_y(char *ad, int size_mem);
extern CPPCALL YMEM void  *buffer_calloc_y(int num);
extern CPPCALL void   buffer_init_calloc_extern(char *ad, int size_mem);
extern CPPCALL EXTERN void  *buffer_calloc_extern(int num);
extern CPPCALL void   buffer_init_calloc_esram(char *ad, int size_mem);
#ifdef __flexcc2__
extern CPPCALL ESRAM void  *buffer_calloc_esram(int num);
#else // __flexcc2__
extern CPPCALL EXTERN void  *buffer_calloc_esram(int num);
#endif // __flexcc2__

#endif // _NMF_MPC_

#endif // _vector_h_ 
