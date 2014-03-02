/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

/*****************************************************************************/
/**
 * This is a Nomadik Multiprocessing Framework generated file, please do not modify it.
 */

/* 'noise_reduction.nmfil.host.effect.configure' interface */
#if !defined(noise_reduction_nmfil_host_effect_configure_IDL)
#define noise_reduction_nmfil_host_effect_configure_IDL

#include <cpp.hpp>

typedef struct t_xyuv_noise_reduction_config_t {
  t_sint16 enabled;
  t_sint16 use_separate_channel_settings;
  t_sint16 adaptive_high_pass_filter;
  t_sint16 desired_noise_reduction;
  t_sint16 nr_cut_off_frequency;
  t_sint16 noise_reduction_during_speech;
  t_sint16 noise_reduction_tradeoff;
  t_sint16 noise_floor_power;
  t_sint16 high_pass_filter_snr_threshold;
  t_sint16 high_pass_cut_off_margin;
  t_sint16 adaptive_high_pass_filter_channel2;
  t_sint16 desired_noise_reduction_channel2;
  t_sint16 nr_cut_off_frequency_channel2;
  t_sint16 noise_reduction_during_speech_channel2;
  t_sint16 noise_reduction_tradeoff_channel2;
  t_sint16 noise_floor_power_channel2;
  t_sint16 high_pass_filter_snr_threshold_channel2;
  t_sint16 high_pass_cut_off_margin_channel2;
} noise_reduction_config_t;


class noise_reduction_nmfil_host_effect_configureDescriptor: public NMF::InterfaceDescriptor {
  public:
    virtual void setConfig(noise_reduction_config_t config) = 0;
};

class Inoise_reduction_nmfil_host_effect_configure: public NMF::InterfaceReference {
  public:
    Inoise_reduction_nmfil_host_effect_configure(NMF::InterfaceDescriptor* _reference = 0):
      InterfaceReference(_reference) {}

    void setConfig(noise_reduction_config_t config) {
      ((noise_reduction_nmfil_host_effect_configureDescriptor*)reference)->setConfig(config);
    }
};

#endif
