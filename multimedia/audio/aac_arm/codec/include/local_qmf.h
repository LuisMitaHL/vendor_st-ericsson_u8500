
#ifndef __LOCAL_QMF_H
#define __LOCAL_QMF_H

#include "audiolibs_common.h"

#define QMF_OK						0
#define NB_CHANNELS_NOT_SUPPORTED 	1


void
cosMod (Float *subband, Float YMEM *scratch_ptr_y, int nb_channels);

void
sinMod (Float *subband, Float YMEM *scratch_ptr_y, int nb_channels);

void
fwd_modulation_post_twiddle(Float *sb_re, Float *sb_im, int length,  Float const *xcos_sin);

extern
void ps_process_slot(void * ps_dec,
                     Float **rIntBufferLeft,
                     Float **iIntBufferLeft,
                     Float *qmfRealTmp,
                     Float *qmfImagTmp,
                     int l,
                     int *env,
                     int usb
#ifdef MMDSP
                     ,int lsb,
                     SBR_SCALING *sbr_scaling
#endif
                     );

#endif
