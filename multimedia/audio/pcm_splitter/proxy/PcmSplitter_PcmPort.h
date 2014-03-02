/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _PCMSPLITTER_PCMPORT_H_
#define _PCMSPLITTER_PCMPORT_H_

#include "AFM_PcmPort.h"
#include "audio_chipset_api_index.h"
#include "audio_codecs_chipset_api.h"
#include "AFM_Symbian_AudioExt.h"

class PcmSplitter_PcmPort: public AFM_PcmPort {
    public:
        PcmSplitter_PcmPort(
                OMX_U32 nIndex,
                OMX_DIRTYPE eDir,
                OMX_U32 nBufferSizeMin,
                const OMX_AUDIO_PARAM_PCMMODETYPE &defaultPcmSettings,
                ENS_Component &enscomp);

        virtual OMX_ERRORTYPE setParameter(
                OMX_INDEXTYPE nParamIndex,
                OMX_PTR pComponentParameterStructure);

        virtual OMX_ERRORTYPE getParameter(
                OMX_INDEXTYPE nParamIndex,
                OMX_PTR pComponentParameterStructure) const;

        inline OMX_BOOL isPaused(void) const {
            return mIsPaused;
        }

        inline void setPause(OMX_BOOL bIsPaused){
            mIsPaused = bIsPaused;
        };

        OMX_ERRORTYPE   setChannelSwitchBoard(OMX_U32 *channelSwitchBoard);
        void            getChannelSwitchBoard(OMX_U32 *channelSwitchBoard) const;


    private:
        virtual OMX_ERRORTYPE checkPcmSettings(
                const OMX_AUDIO_PARAM_PCMMODETYPE &pcmSettings) const;

        OMX_ERRORTYPE   checkChannelSwitchBoardConfig(const OMX_U32 *channelSwitchBoard) const;


        OMX_BOOL    mIsSynchronized;
        OMX_BOOL    mIsPaused;
        OMX_U32     mChannelSwitchBoard[OMX_AUDIO_MAXCHANNELS];
};

typedef PcmSplitter_PcmPort* PcmSplitter_PcmPort_p;

#endif // _PCMSPLITTER_PCMPORT_H_
