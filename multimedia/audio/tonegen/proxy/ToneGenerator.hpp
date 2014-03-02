/*****************************************************************************/
/**
*  © ST-Ericsson, 2010 - All rights reserved

*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Tone generator proxy
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef _TONEGENERATOR_HPP_
#define _TONEGENERATOR_HPP_

#include "ENS_Component.h"
#include "AFM_Component.h"
#include "OMX_Types.h"
#include "audio_sourcesink_chipset_api.h"
#include "tonegen/nmfil/host/composite/tonegen.hpp"

#include <math.h>


#define TONEGENERATOR_ENABLE_DEFAULT (OMX_TRUE)
#define TONEGENERATOR_TONETYPE_DEFAULT (OMX_TONEGEN_DtmfTone)
#define TONEGENERATOR_FREQ1_DEFAULT (0)
#define TONEGENERATOR_FREQ2_DEFAULT (0)
#define TONEGENERATOR_DIGIT_DEFAULT ('0')

#define TONEGENERATOR_VOLUME_DEFAULT (0x7FFF)
#define TONEGENERATOR_MUTE_DEFAULT (OMX_FALSE)

#define TONEGENERATOR_VOLUME_MAX_Q15 (0x7FFFF)
#define TONEGENERATOR_VOLUME_MIN_Q15 (0)
#define TONEGENERATOR_VOLUME_MAX_MB (0)
#define TONEGENERATOR_VOLUME_MIN_MB (-10000)
#define TONEGENERATOR_VOLUME_MAX_PERCENT (100)
#define TONEGENERATOR_VOLUME_MIN_PERCENT (0)



#define CONV_PERCENT_TO_Q(Q15, x) \
  do { \
    if ((x) == 100) \
      Q15 = 0x7FFF; \
    else \
      Q15 = (t_sint16)(((t_sint32) (x) << 15) / 100); \
  } while (0)

#define CONV_Q_TO_PERCENT(x, Q15) \
  x = (t_sint16)(((t_sint32) (Q15) * 100 + 0x4000) >> 15)

#define CONV_MB_TO_Q(Q15, x) \
  Q15 = (t_sint16)0x7FFF*powf(10.0,(float)(x)/2000.0)

#define CONV_Q_TO_MB(x, Q15) \
  do { \
    if ((Q15) == 0) \
      x = TONEGENERATOR_VOLUME_MIN_MB; \
    else \
      x = (t_sint16)2000*log10f((float)(Q15)/0x7FFF); \
  } while (0)



/**
 * @brief Factory method for the Tone generator ENS component
 *
 * This method creates a Tone generator ENS component.
 * It is used by the ENS Core to create a Tone generator OMX IL component.
 * Register this method to ENS Core will enable tone generator OMX IL support.
 */
OMX_ERRORTYPE tonegeneratorFactoryMethod(ENS_Component_p *ppENSComponent);

/**
 * @brief Class that implements the generic part of the tone generator
 *        ENS/AFM component
 */
class ToneGenerator : public AFM_Component {
public:

    /** @brief Constructor */
    OMX_ERRORTYPE  construct(void);
    /** @brief Destructor */
    ~ToneGenerator() {}

    OMX_ERRORTYPE getParameter(
                OMX_INDEXTYPE nParamIndex,
                OMX_PTR pComponentParameterStructure) const;

    /**
     * @brief Set config struct for Tone generator.
     */
    OMX_ERRORTYPE setConfig(
                OMX_INDEXTYPE nConfigIndex,
                OMX_PTR pComponentConfigStructure);

    /**
     * @brief Get config struct for Tone generator.
     */
    OMX_ERRORTYPE getConfig(
                OMX_INDEXTYPE nConfigIndex,
                OMX_PTR pComponentConfigStructure) const;

    OMX_U32 getBlockSize() const { return DEFAULT_PCM_BLOCKSIZE; }

    /** @brief Returns sample bit size. */
    OMX_U32 getSampleBitSize(void) const { return 16; }

    OMX_U32 getChannels(void) const;
    void getVolumeLeft(t_sint16* startVolume,
                       t_sint16* endVolume,
                       t_bool* muteState) const;
    void getVolumeRight(t_sint16* startVolume,
                        t_sint16* endVolume,
                        t_bool* muteState) const;
    void getTone(Tone_t* tone) const;

private:
    OMX_BOOL mEnable;
    OMX_TONEGENTONETYPE mTonetype;
    OMX_U32 mFrequency[2];
    OMX_U32 mDigit;
    t_sint16 mLeftVolume[2];
    t_sint16 mRightVolume[2];
    OMX_BOOL mLeftMute;
    OMX_BOOL mRightMute;
};

#endif // _TONEGENERATOR_HPP_
