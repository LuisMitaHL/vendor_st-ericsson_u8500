/*****************************************************************************/
/**
*  © ST-Ericsson, 2010 - All rights reserved

*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   DRC Host NMF effect interface class
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef _DRC_NMF_HOST_EFFECT_INC_EFFECT_HPP_
#define _DRC_NMF_HOST_EFFECT_INC_EFFECT_HPP_

#include "drc.h"
#include "metadata.hpp"


class drc_nmfil_host_effect : public drc_nmfil_host_effectTemplate
{
   public:
      drc_nmfil_host_effect();
     ~drc_nmfil_host_effect();

      // Effect Interface
      virtual t_bool open(const t_host_effect_config* config, t_effect_caps* caps);
      virtual void reset(t_effect_reset_reason reason);
      virtual void process(t_effect_process_params* params);
      virtual void close(void);

      // Configure Interface
      virtual void setConfig(t_drc_config config);

   private:
      t_uint16                  mEOS;                        /**< EOS reached */
      t_bool                    mEnable;                     /**< Set DRC algorithm active/inactive */
      t_bool                    mUseSeparateChannelSettings; /**< Use separate settings for lrft and right channel */
      t_drc_handle             *pHandle;                     /**< Handler to create DRC */
      t_drc_handle             *pHandle2;                    /**< Handler to create DRC */
      t_drc_param               mConfig;                     /**< Store configuration changed during runtime */
      t_drc_param               mConfigChannel2;             /**< Store configuration changed during runtime */
      t_drc_allocation_params   mParams;                     /**< Hold allocation and system parameters */
      t_uint32                  mBufferSize;                 /**< Number of bits in the frame */
      omx_metadata             *pOMXMetadata;                /**< Buffer metadata array */
      t_uint16                  mOMXSize;                    /**< Number of metadata objects being allocated in the Circular Array */
      t_uint16                  mOMXReadIndex;               /**< Index to insert OMX metadata */
      t_uint16                  mOMXWriteIndex;              /**< Index to retrieve OMX metadata */
      t_uint8                   mFlag;                       /**< Flags to set gain state at the start up */
      
      t_sint16 *m_in_buf_p[2];                               /**< Used for processing separate channels */
      t_sint16 *m_out_buf_p[2];                              /**< Used for processing separate channels */

      t_uint16 getSampleRate(const t_sample_freq Fs);
      t_bool   recreateAlgorithm();

};

#endif /* _DRC_NMF_HOST_EFFECT_INC_EFFECT_HPP_ */
