/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*****************************************************************************
 * \note 	copied from audiolibs/common/include/vector.h
 *  
 */
/*****************************************************************************/

#ifndef _VECTOR_H_
#define _VECTOR_H_

/* #include "audiolibs_common.h" */

/* extern CPPCALL void vector_set_value(Float *p,int len,Float value,int incr); */
/* extern CPPCALL void vector_window(Float *p,Float YMEM *win,int len, */
/* 			  int incr1,int incr2); */
/* extern CPPCALL void vector_window_copy(Float *p, Float *q, Float FT_MEM *win, int len, */
/* 			       int incr1, int incr2, int incr3); */
/* extern CPPCALL void vector_window_add(Float *p, Float *q, Float FT_MEM *win, int len, */
/* 			      int incr1, int incr2, int incr3); */
/* extern CPPCALL void vector_window_sub(Float *p, Float *q, Float FT_MEM *win, int len, */
/* 			      int incr1, int incr2, int incr3); */
/* extern CPPCALL void  vector_copy(Float *p,Float *q,int len,int incr1,int incr2); */
/* #ifndef __flexcc2__ */
/* extern CPPCALL void  vector_copy_from_waitmem(Float *p, Float const WAITMEM *q, int len); */
/* extern CPPCALL void  vector_copy_to_y_from_waitmem(Float YMEM *p, Float const WAITMEM *q, int len); */
/* #endif /\* __flexcc2__ *\/ */
/* extern CPPCALL void  vector_copy_neg(Float *p,Float *q,int len,int incr1,int incr2); */
/* extern CPPCALL void  vector_swap(Float *p,Float *q,int len,int incr1,int incr2); */
/* extern CPPCALL void  vector_add(Float *p,Float *q,int len,int incr1, int incr2); */
/* extern CPPCALL void  vector_sub(Float *p, Float *q, int len, int incr1, int incr2); */
/* extern CPPCALL void  vector_add_copy(Float *res,Float *p,Float *q,int len,int incr); */
/* extern CPPCALL void  vector_ms(Float *m,Float *s,int len); */
/* extern CPPCALL void  vector_ms_scale(Float *m,Float *s,Float scale, int len); */
/* extern CPPCALL void  vector_scale(Float *src,Float *dest,Float scale,int len); */
/* extern CPPCALL void  vector_scale_in_place(Float *p, Float scale, int len, int incr); */
/* extern CPPCALL void  vector_abs(Float *p,Float *q,int len,int incr1); */
/* extern CPPCALL Float vector_max_magnitude(Float *src1, int len, int incr); */
/* extern CPPCALL void  vector_scale_copy_to_buffers(Float *data_ptr,Float **result_ptr, */
/* 				  int block_len,Float scale); */
/* extern CPPCALL void  vector_scale_add(Float **data_ptr,Float *result_ptr,int block_len, */
/* 				  Float scale1,Float scale2); */
/* extern CPPCALL void  vector_scale_mix(Float **data_ptr,Float **result_ptr, 
   int block_len, Float *scale);*/

extern  void   buffer_reset(char *p, char val, int size);

/* extern CPPCALL void   buffer_copy(char *p, char *q, int size); */
/* #ifndef __flexcc2__ */
/* extern CPPCALL void   buffer_copy_from_waitmem(char *p, char const WAITMEM *q, int size); */
/* extern CPPCALL void   buffer_copy_to_y_from_waitmem(char YMEM *p, char const WAITMEM *q, int size); */
/* #endif /\* __flexcc2__ *\/ */
/* extern CPPCALL void   buffer_init_calloc(char *adr,int size_mem); */
/* extern CPPCALL void  *buffer_calloc(int num,int size); */


#endif /* _VECTOR_H_ */
