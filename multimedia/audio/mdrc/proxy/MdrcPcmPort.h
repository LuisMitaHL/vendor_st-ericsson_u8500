/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   MdrcPcmPort.h
 * \brief  
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _MDRCPCMPORT_H_
#define _MDRCPCMPORT_H_

#include "AFM_PcmPort.h"

class MdrcPcmPort: public AFM_PcmPort {
    public:
        MdrcPcmPort(OMX_U32         nIndex,
                    OMX_DIRTYPE     eDir,
                    OMX_U32         nChannels,
                    OMX_U32         nSampleRate,
                    OMX_U32         nBitPerSample,
                    ENS_Component & enscomp):AFM_PcmPort(nIndex,
                                                         eDir,
                                                         nChannels,
                                                         nSampleRate,
                                                         nBitPerSample,
                                                         enscomp) {};
        MdrcPcmPort(OMX_U32 nIndex,
                    OMX_DIRTYPE eDir,
                    OMX_U32 nBufferSizeMin,
                    const OMX_AUDIO_PARAM_PCMMODETYPE &defaultPcmSettings,
                    ENS_Component &enscomp):AFM_PcmPort(nIndex,
                                                        eDir,
                                                        nBufferSizeMin,
                                                        defaultPcmSettings,
                                                        enscomp) {};
        //~MdrcPcmPort() {};
    private:
        virtual OMX_ERRORTYPE   checkPcmSettings(const OMX_AUDIO_PARAM_PCMMODETYPE &pcmSettings) const;
};

#endif
