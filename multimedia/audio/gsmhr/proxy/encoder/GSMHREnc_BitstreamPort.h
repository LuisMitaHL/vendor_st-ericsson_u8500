/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _GSMHRENC_BITSTREAMPORT_H_
#define _GSMHRENC_BITSTREAMPORT_H_

#include "AFM_Port.h"

class GSMHREnc_BitstreamPort: public AFM_Port {
    public:
        GSMHREnc_BitstreamPort(
                OMX_U32 nIndex,
                OMX_DIRTYPE eDir,
                OMX_U32 nBufferSizeMin,
                const OMX_AUDIO_PARAM_GSMHRTYPE &defaultGsmhrSettings,
                ENS_Component &enscomp);

        virtual OMX_ERRORTYPE setParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure);

        virtual OMX_ERRORTYPE getParameter(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentParameterStructure) const;

        virtual OMX_ERRORTYPE checkFormatCompatibility(OMX_HANDLETYPE hTunneledComp, OMX_U32 nTunneledPort) const;

        void checkInformationGivenByPcmPort(OMX_AUDIO_PARAM_PCMMODETYPE pcmformat);
    private:
        OMX_ERRORTYPE CheckOMXParams(OMX_AUDIO_PARAM_GSMHRTYPE *gsmhrParams);

        OMX_AUDIO_PARAM_GSMHRTYPE mGsmhrParams;
};

typedef GSMHREnc_BitstreamPort * GSMHREnc_BitstreamPort_p;

#endif // _AFM_BITSTREAMPORT_H_
