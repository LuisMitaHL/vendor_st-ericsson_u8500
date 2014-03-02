/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   spl_dsp_M4.h
 * \brief  
 * \author ST-Ericsson
 */
#ifndef _spl_dsp_M4_h_
#define _spl_dsp_M4_h_
extern int spl_pow10_coef(int Input); 
extern int spl_log10(long long Input);
extern void spl_pow10(int X, int *exp, int *mant);
#endif /* _spl_dsp_M4__h_ */
