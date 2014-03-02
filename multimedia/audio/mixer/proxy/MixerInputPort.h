/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   MixerInputPort.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _MIXERINPUTPORT_H_
#define _MIXERINPUTPORT_H_

#include "MixerPort.h"
#include "Mixer.h"

class MixerInputPort: public MixerPort {
    public:
        MixerInputPort(OMX_U32 nIndex, ENS_Component &enscomp);

        inline OMX_U16 getSrcMode(void) const {
            return mSrcMode;
        }
        inline OMX_BOOL isPaused(void) const {
            return mIsPaused;
        }

        inline void setPause(OMX_BOOL bIsPaused) {
            mIsPaused = bIsPaused;
        };

        virtual OMX_ERRORTYPE setParameter(
                OMX_INDEXTYPE nParamIndex,
                OMX_PTR pComponentParameterStructure);

        virtual OMX_ERRORTYPE getParameter(
                OMX_INDEXTYPE nParamIndex,
                OMX_PTR pComponentParameterStructure) const;

        virtual OMX_ERRORTYPE setConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure);

        virtual OMX_ERRORTYPE getConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure) const;

        OMX_AUDIO_CONFIG_VOLUMETYPE *getConfigVolume(void)  {return &mConfigVolume;}
        OMX_SYMBIAN_AUDIO_CONFIG_VOLUMERAMPTYPE  *getConfigVolumeRamp(void)  {return &mConfigVolumeRamp;}
        OMX_AUDIO_CONFIG_MUTETYPE *getConfigMute(void)  {return &mConfigMute;}
        OMX_AUDIO_CONFIG_BALANCETYPE *getConfigBalance(void)  {return &mConfigBalance;}

    private:
        void setDefaultSettings(void);

        OMX_BOOL                                    mIsPaused;
        //SRC configuration
        AFM_SRC_MODE                                mSrcMode;
        //OMX volume configurations
        OMX_BOOL                                    mIsRampVolume;
        OMX_AUDIO_CONFIG_VOLUMETYPE                 mConfigVolume;
        OMX_SYMBIAN_AUDIO_CONFIG_VOLUMERAMPTYPE     mConfigVolumeRamp;
        OMX_AUDIO_CONFIG_MUTETYPE                   mConfigMute;
        OMX_AUDIO_CONFIG_BALANCETYPE                mConfigBalance;
};

#endif // _MIXERINPUTPORT_H_
