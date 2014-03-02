/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   spl_dsp.h
 * \brief  
 * \author ST-Ericsson
 */
#ifndef _spl_dsp_h_
#define _spl_dsp_h_


// functions
extern Word24 spl_pow10_coef(Word24 Input); 
extern Word24 spl_log10(Word48 Input);
extern void spl_pow10(MMshort X, MMshort *exp, MMshort *mant);

#endif /* _spl_dsp_h_ */
