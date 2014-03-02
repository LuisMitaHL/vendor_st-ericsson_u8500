/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */
/**
 * \file   spl_util_M4.h
 * \brief  
 * \author ST-Ericsson
 */
#ifndef _spl_util_M4_h_
#define _spl_util_M4_h_
void spl_read(SPL_LOCAL_STRUCT_T *Heap);
void spl_write(SPL_LOCAL_STRUCT_T *Heap);
void spl_move(SPL_LOCAL_STRUCT_T * HeapPtr);
int spl_check_param_int(int *p_val, int *p_val_in, int min, int max);
int spl_check_param_uint(unsigned int *p_val, unsigned int *p_val_in,
                         unsigned int min, unsigned int max);
#endif /* _spl_util_h_ */
