/*****************************************************************************/
/**
*  © ST-Ericsson, 2010 - All rights reserved
*
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson.
*
* \brief   Virtual Surround Host NMF effect interface class
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef _VIRTUAL_SURROUND_NMF_HOST_EFFECT_INC_EFFECT_HPP_
#define _VIRTUAL_SURROUND_NMF_HOST_EFFECT_INC_EFFECT_HPP_

#include "virtual_surround.h"

class virtual_surround_nmfil_host_effect : public virtual_surround_nmfil_host_effectTemplate
{
   public:
      virtual_surround_nmfil_host_effect();
     ~virtual_surround_nmfil_host_effect();

      // Effect Interface
      virtual t_bool open(const t_host_effect_config* config, t_effect_caps* caps);
      virtual void reset(t_effect_reset_reason reason);
      virtual void process(t_effect_process_params* params);
      virtual void close(void);

      // Configure Interface
      virtual void setConfig(t_virtual_surround_config config);

   private:
      t_bool                   mEnable;                 // Set Virtual Surround algorithm active/inactive
      t_uint16                 mInChannels;             // Number of channels encoded in the multichannel stream
      t_uint16                 mOutChannels;            // Number of channels delivered to the sink
      t_uint32                 mBlockSize;              // Number of sample per channel
      t_vs_handle             *pHandle;                 // Virtual Surround handle
      t_vs_param               mConfig;                 // Virtual Surround configuration
      t_vs_allocation_params   mParams;                 // Allocation and system parameters
      t_uint32                 mBufferSize;             // Number of bytes in the frame

      t_uint16 getSampleRate(const t_sample_freq Fs);

};

#endif /* _VIRTUAL_SURROUND_NMF_HOST_EFFECT_INC_EFFECT_HPP_ */
