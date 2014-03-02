/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved

*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Noise Reduction Host nmf processing class
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef _noise_reduction_nmfil_host_effect_hpp_
#define _noise_reduction_nmfil_host_effect_hpp_

extern "C" {
#include "noise_reduction.h"
}

/**
 * @brief Class that implements the Noise Reduction NMF component for the host CPU.
 */
class noise_reduction_nmfil_host_effect : public noise_reduction_nmfil_host_effectTemplate
{
public:

    noise_reduction_nmfil_host_effect();
    ~noise_reduction_nmfil_host_effect() {}

    // Effect interface
    t_bool open(const t_host_effect_config* config, t_effect_caps* caps);
    void reset(t_effect_reset_reason reason);
    void process(t_effect_process_params* params);
    void close(void);

    // chorus interface
    void setConfig(noise_reduction_config_t config);

private:
    int convert_sample_rate(t_sample_freq fs);

    t_bool m_use_separate_channel_settings;
    int m_nof_channels;
    int m_samplerate;
    int m_block_size;

    short *m_in_buf_p[2];
    short *m_out_buf_p[2];

    noise_reduction_handle_t *nr_handle_p;
    noise_reduction_handle_t *nr2_handle_p;

    // effect configuration
    SE_NrParameters_t m_nr_config;
    SE_NrParameters_t m_nr2_config;
};

#endif // _noise_reduction_nmfil_host_effect_hpp_
