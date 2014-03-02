/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved

*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   AMR Encoder Bitstream Port class
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef _AMRENC_BITSTREAMPORT_H_
#define _AMRENC_BITSTREAMPORT_H_

#include "AFM_Port.h"

class AMREnc_BitstreamPort: public AFM_Port {
    public:
        AMREnc_BitstreamPort(
                OMX_U32 nIndex,
                OMX_DIRTYPE eDir,
                OMX_U32 nBufferSizeMin,
                const OMX_AUDIO_PARAM_AMRTYPE &defaultAmrSettings,
                bool isHost,
                ENS_Component &enscomp);

        virtual OMX_ERRORTYPE setParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure);

        virtual OMX_ERRORTYPE getParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure) const;

        virtual OMX_ERRORTYPE checkFormatCompatibility(OMX_HANDLETYPE hTunneledComp, OMX_U32 nTunneledPort) const;

        void checkInformationGivenByPcmPort(OMX_AUDIO_PARAM_PCMMODETYPE pcmformat);

        OMX_ERRORTYPE setBitRate(OMX_AUDIO_AMRBANDMODETYPE OmxBandMode);
        OMX_AUDIO_AMRDTXMODETYPE getDtx();
        OMX_AUDIO_AMRFRAMEFORMATTYPE getFrameFormat();

    private:
        OMX_ERRORTYPE CheckOMXParams(OMX_AUDIO_PARAM_AMRTYPE *amrParams);

        OMX_AUDIO_PARAM_AMRTYPE mAmrParams;
};

typedef AMREnc_BitstreamPort * AMREnc_BitstreamPort_p;

#endif // _AFM_BITSTREAMPORT_H_
