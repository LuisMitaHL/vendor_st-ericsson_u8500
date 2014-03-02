
 
#ifndef _QMF_H_
#define _QMF_H_

#include "fft.h"
#include "imdct.h"
#include "audiolibs_common.h"
#include "common_interface.h"
#include "fwmalloc.h"

typedef struct
{
    int qmf_ana_scale[38];
    int lb_scale;
    int hb_scale;
    int ov_lb_scale;
    int ov_hb_scale;
    int ps_scale;
}
SBR_SCALING;

int
InitSbrAnaFilterbank(int nb_subbands); 

int
SbrAnaFilterbank( Float const * timeSig,
                  Float ** Sr,
                  Float ** Si,
                  int chan,
                  int nb_channels,
                  int maxBand,
                  Float gain);

void
InitSbrSynFilterbank(int bDownSampleSBR, Float gain); 

void
SbrSynFilterbank( Float ** Sr,
                  Float ** Si,
                  Float * timeSig,
                  int bDownSampledSbr,
                  int channel
                  );
int
InitSbrAnaFilterbank_lp(int nb_subbands); 

int
SbrAnaFilterbank_lp( Float const * timeSig,
                  Float ** Sr,
                  int chan,
                  int nb_channels,
                  int maxBand,
                  Float gain);

void
InitSbrSynFilterbank_lp(int bDownSampleSBR, Float gain); 

void
SbrSynFilterbank_lp( Float ** Sr,
                  Float * timeSig,
                  int bDownSampledSbr,
                  int channel
                  );

CODEC_INIT_T
Open_SbrAnaFilterbank_opt(Float **qmf_states_buffer,
                          int nb_subbands,
                          void *add_ptr
                          );

void
Reset_SbrAnaFilterbank_opt(Float *qmf_state_ptr, int nb_channels);

int
SbrAnaFilterbank_opt(const Float *timeIn,
                         Float **rAnalysis,
                         Float **iAnalysis,
                         Float *qmf_states_buffer,
                         Float YMEM *scratch_ptr_y,
                         int nb_subbands,
                         int gain_shift,
                         int lsb,
                         int *scale);
int
SbrAnaFilterbank_opt_lp(const Float *timeIn,
                         Float **rAnalysis,
                         Float *qmf_states_buffer,
                         Float YMEM *scratch_ptr_y,
                         int nb_subbands,
                         int gain_shift,
                         int lsb,
                         int *scale);

CODEC_INIT_T
Open_SbrSynFilterbank_opt(Float **timeBuffer,
                          int nb_subbands,
                          void *add_ptr
                        );


void
Reset_SbrSynFilterbank_opt(Float *state_ptr, int nb_channels);

void
SbrSynFilterbank_opt(Float **sbrReal,
                     Float **sbrImag,
                     Float *timeOut,
                     Float *timeBuffer,
                     Float YMEM *scratch_ptr_y,
                     int nb_channels,
#ifdef MMDSP
                     int lsb,
                     int usb,
                     SBR_SCALING *sbr_scaling,
                     int gain_shift,
#else
                     int usb,
                     int gain_shift,
#endif
                     void * ps_dec,
                     int active
#ifndef MMDSP 
 ,void  (*ps_process_slot_ptr_aac)(void* ps_dec,
                      Float **rIntBufferLeft,
                      Float **iIntBufferLeft,
                      Float *qmfRealTmp,
                      Float *qmfImagTmp,
                      int l,
                      int *env,
                      int usb
                      )
#endif
                     );
void
SbrSynFilterbank_opt_lp(Float **sbrReal,
                     Float *timeOut,
                     Float *timeBuffer,
                     Float YMEM *scratch_ptr_y,
                     int nb_channels,
#ifdef MMDSP
                     int lsb,
                     int usb,
                     SBR_SCALING *sbr_scaling,
                     int gain_shift,
#else
                     int usb,
                     int gain_shift,
#endif
                     void *ps_dec,
                     int ps_active);


#endif // _QMF_H_
