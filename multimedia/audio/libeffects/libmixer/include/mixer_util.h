/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   mixer_util.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _mixer_util_h_
#define _mixer_util_h_

#ifdef MMDSP

MMlong mixer_short_long_mult(MMshort short_var, MMlong long_var);

MMshort mixer_divfract(MMshort n, MMshort d);

MMshort mixer_envelop_detection(MMlong input_sample, MIXER_LOCAL_STRUCT_T *mixer, MMshort channel);

MMshort mixer_compute_gain(MIXER_LOCAL_STRUCT_T *mixer, MMshort channel);

void mixer_poly_horner_eval(MMshort x,MMlong *coef,MMshort *y,MMshort order);

#else

MMlong mixer_envelop_detection(MMlong input_sample, MIXER_LOCAL_STRUCT_T *mixer, MMshort channel);

MMlong mixer_compute_gain(MIXER_LOCAL_STRUCT_T *mixer, MMshort channel);

void mixer_poly_horner_eval(MMlong x,MMlong *coef,MMlong *y,MMshort order);

#endif
#endif /* _mixer_util_h_ */
