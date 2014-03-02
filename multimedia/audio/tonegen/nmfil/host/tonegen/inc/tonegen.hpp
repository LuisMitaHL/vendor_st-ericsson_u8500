/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2010 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief
* \author ST-Ericsson
*/
/*****************************************************************************/
#ifndef _tonegen_nmfil_host_tonegen_hpp_
#define _tonegen_nmfil_host_tonegen_hpp_

#include "t_tonegen.h"

/**
 * @brief Class that implements the Tone genenrator NMF component for the host CPU.
 */
class tonegen_nmfil_host_tonegen : public tonegen_nmfil_host_tonegenTemplate
{
public:

  tonegen_nmfil_host_tonegen();
  ~tonegen_nmfil_host_tonegen() { };

  // Source interface
  t_bool open(const t_source_config* config);
  void reset(t_source_reset_reason reason);
  void process(t_source_process_params* params);
  void close(void);

  // Tone generator interface
  void setTone(Tone_t tone);
  void setVolume(t_sint16 volume, t_uint32 nChannel);
  void setVolumeRamp(t_sint16 startVolume, t_sint16 endVolume, t_uint32 nChannel);
  void setMute(t_bool muteState, t_uint32 nChannel);
  void setConfig(Config_t config);

private:
  void applySettings(void);

  TG_Handle_t* mHandle;
  TG_PCM_Config_t* mPCM_Config;

  t_bool mEnable;
  t_tonegen_tone_type mTone_type;
  t_uint32 mFrequencies[2];
  TG_Tone_t mDigit;
  t_sint16 mVolume[2];
  t_bool mMute[2];
  t_bool mReset;
  Config_t mConfig;
};

#endif // _tonegen_nmfil_host_tonegen_hpp_
