/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   AMRDec_PcmPort.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _AMRDEC_PCMPORT_H_
#define _AMRDEC_PCMPORT_H_

#include "AFM_PcmPort.h"

class AMRDec_PcmPort: public AFM_PcmPort {
    public:
        AMRDec_PcmPort(OMX_U32         nIndex,
                       OMX_DIRTYPE     eDir,
                       OMX_U32         nChannels,
                       OMX_U32         nSampleRate,
                       OMX_U32         nBitPerSample,
                       bool            isHost,
                       ENS_Component & enscomp);
        AMRDec_PcmPort(OMX_U32 nIndex,
                    OMX_DIRTYPE eDir,
                    OMX_U32 nBufferSizeMin,
                    const OMX_AUDIO_PARAM_PCMMODETYPE &defaultPcmSettings,
                    bool isHost,
                    ENS_Component &enscomp);
        //~AMRDec_PcmPort() {};
    private:
        virtual OMX_ERRORTYPE   checkPcmSettings(const OMX_AUDIO_PARAM_PCMMODETYPE &pcmSettings) const;
        void setProperties(bool isHost);
};

#endif
