/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved

*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Noise Reduction pcm processor proxy
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef _NOISE_REDUCTION_PCM_PROCESSING_H_
#define _NOISE_REDUCTION_PCM_PROCESSING_H_

#include "AFM_PcmProcessing.h"


/**
 * Factory method for the Noise Reduction ENS component
 *
 * This method creates an Noise Reduction ENS component.
 * It is used by the ENS Core to create an Noise Reduction OMX IL component.
 * Register this method to ENS Core will enable Noise Reduction OMX IL support.
 */
OMX_ERRORTYPE NoiseReductionPcmProcFactoryMethod(ENS_Component_p *ppENSComponent);

/**
 * Class that implements the generic part of the Noise Reduction ENS/AFM component
 */
class NoiseReductionPcmProcessing : public AFM_PcmProcessing {

public:

    /** Construct */
    OMX_ERRORTYPE  construct(void);
    /** Destructor */
    virtual ~NoiseReductionPcmProcessing() {}

    /** setConfig for Noise Reduction. */
    virtual OMX_ERRORTYPE setConfig(
                OMX_INDEXTYPE nConfigIndex,
                OMX_PTR pComponentConfigStructure);

    /** getConfig for Noise Reduction. */
    virtual OMX_ERRORTYPE getConfig(
                OMX_INDEXTYPE nConfigIndex,
                OMX_PTR pComponentConfigStructure) const;

    /** getParam for Noise Reduction */
    virtual OMX_ERRORTYPE getParameter(
            OMX_INDEXTYPE nParamIndex,  
            OMX_PTR pComponentParameterStructure) const;

    /** setParam for Noise Reduction */
    virtual OMX_ERRORTYPE setParameter(
            OMX_INDEXTYPE nParamIndex,
            OMX_PTR pComponentParameterStructure);
            
    /** Get bits per sample. */
    virtual OMX_U32 getSampleBitSize(void) const { return 16; }
    
    /** Get block size */
    OMX_U32 getBlockSize() const;
    // ejohsan

    /** Get enable status. */
    OMX_BOOL get_enable(void) const { return m_enable; }

    /** Get enable status. */
    OMX_BOOL get_use_separate_channel_settings(void) const { return m_use_separate_channel_settings; }


    /** Get adaptive high pass filter status. */
    OMX_S16 get_adaptive_high_pass_filter(void) const { return m_adaptive_high_pass_filter; }

    /** Get desired noise reduction status. */
    OMX_S16 get_desired_noise_reduction(void) const { return m_desired_noise_reduction; }

    /** Get nr cut off frequency status. */
    OMX_S16 get_nr_cut_off_frequency(void) const { return m_nr_cut_off_frequency; }

    /** Get noise reduction during speech status. */
    OMX_S16 get_noise_reduction_during_speech(void) const { return m_noise_reduction_during_speech; }

    /** Get noise reduction tradeoff status. */
    OMX_S16 get_noise_reduction_tradeoff(void) const { return m_noise_reduction_tradeoff; }

    /** Get noise floor power status. */
    OMX_S16 get_noise_floor_power(void) const { return m_noise_floor_power; }

    /** Get high pass filter snr threshold status. */
    OMX_S16 get_high_pass_filter_snr_threshold(void) const { return m_high_pass_filter_snr_threshold; }

    /** Get high pass cut off margin status. */
    OMX_S16 get_high_pass_cut_off_margin(void) const { return m_high_pass_cut_off_margin; }


    /** Get adaptive high pass filter status. */
    OMX_S16 get_adaptive_high_pass_filter_channel2(void) const { return m_adaptive_high_pass_filter_channel2; }

    /** Get desired noise reduction status. */
    OMX_S16 get_desired_noise_reduction_channel2(void) const { return m_desired_noise_reduction_channel2; }

    /** Get nr cut off frequency status. */
    OMX_S16 get_nr_cut_off_frequency_channel2(void) const { return m_nr_cut_off_frequency_channel2; }

    /** Get noise reduction during speech status. */
    OMX_S16 get_noise_reduction_during_speech_channel2(void) const { return m_noise_reduction_during_speech_channel2; }

    /** Get noise reduction tradeoff status. */
    OMX_S16 get_noise_reduction_tradeoff_channel2(void) const { return m_noise_reduction_tradeoff_channel2; }

    /** Get noise floor power status. */
    OMX_S16 get_noise_floor_power_channel2(void) const { return m_noise_floor_power_channel2; }

    /** Get high pass filter snr threshold status. */
    OMX_S16 get_high_pass_filter_snr_threshold_channel2(void) const { return m_high_pass_filter_snr_threshold_channel2; }

    /** Get high pass cut off margin status. */
    OMX_S16 get_high_pass_cut_off_margin_channel2(void) const { return m_high_pass_cut_off_margin_channel2; }

private:
    // config parameters
    OMX_BOOL m_enable;
    OMX_BOOL m_use_separate_channel_settings;
    OMX_S16  m_adaptive_high_pass_filter;
    OMX_S16  m_desired_noise_reduction;
    OMX_S16  m_nr_cut_off_frequency;
    OMX_S16  m_noise_reduction_during_speech;
    OMX_S16  m_noise_reduction_tradeoff;
    OMX_S16  m_noise_floor_power;
    OMX_S16  m_high_pass_filter_snr_threshold;
    OMX_S16  m_high_pass_cut_off_margin;

    OMX_S16  m_adaptive_high_pass_filter_channel2;
    OMX_S16  m_desired_noise_reduction_channel2;
    OMX_S16  m_nr_cut_off_frequency_channel2;
    OMX_S16  m_noise_reduction_during_speech_channel2;
    OMX_S16  m_noise_reduction_tradeoff_channel2;
    OMX_S16  m_noise_floor_power_channel2;
    OMX_S16  m_high_pass_filter_snr_threshold_channel2;
    OMX_S16  m_high_pass_cut_off_margin_channel2;

    OMX_U32 input_sample_rate;
    OMX_U32 output_sample_rate;
    int calculate_buffer_size(int sample_rate) const;
    bool ports_created;
};

#endif // _NOISE_REDUCTION_PCM_PROCESSING_H_
