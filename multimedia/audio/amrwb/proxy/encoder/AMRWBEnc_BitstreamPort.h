/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _AMRWBENC_BITSTREAMPORT_H_
#define _AMRWBENC_BITSTREAMPORT_H_

#include "AFM_Port.h"

class AMRWBEnc_BitstreamPort: public AFM_Port {
    public:
        AMRWBEnc_BitstreamPort(
                OMX_U32 nIndex,
                OMX_DIRTYPE eDir,
                OMX_U32 nBufferSizeMin,
                const OMX_AUDIO_PARAM_AMRTYPE &defaultAmrSettings,
                ENS_Component &enscomp);

        virtual OMX_ERRORTYPE setParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure);

        virtual OMX_ERRORTYPE getParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure) const;

        virtual OMX_ERRORTYPE checkFormatCompatibility(OMX_HANDLETYPE hTunneledComp, OMX_U32 nTunneledPort) const;

        void checkInformationGivenByPcmPort(OMX_AUDIO_PARAM_PCMMODETYPE pcmformat);
        OMX_ERRORTYPE setBitRate(OMX_AUDIO_AMRBANDMODETYPE OmxBandMode);
        OMX_AUDIO_AMRDTXMODETYPE getDtx();
        OMX_AUDIO_AMRFRAMEFORMATTYPE getFrameFormat();
    private:
        OMX_ERRORTYPE CheckOMXParams(OMX_AUDIO_PARAM_AMRTYPE *amrwbParams);

        OMX_AUDIO_PARAM_AMRTYPE mAmrwbParams;
};

typedef AMRWBEnc_BitstreamPort * AMRWBEnc_BitstreamPort_p;

#endif // _AFM_BITSTREAMPORT_H_
