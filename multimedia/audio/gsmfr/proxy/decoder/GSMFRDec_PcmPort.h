/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

#ifndef _GSMFRDEC_PCMPORT_H_
#define _GSMFRDEC_PCMPORT_H_

#include "AFM_PcmPort.h"

class GSMFRDec_PcmPort: public AFM_PcmPort {
    public:
       GSMFRDec_PcmPort(
                    OMX_U32 nIndex,
                    OMX_DIRTYPE eDir,
                    OMX_U32 nBufferSizeMin,
                    const OMX_AUDIO_PARAM_PCMMODETYPE &defaultPcmSettings,
                    ENS_Component &enscomp):AFM_PcmPort(nIndex,
                                                        eDir,
                                                        nBufferSizeMin,
                                                        defaultPcmSettings,
                                                        enscomp){};
        
    private:
        virtual OMX_ERRORTYPE   checkPcmSettings(const OMX_AUDIO_PARAM_PCMMODETYPE &defaultPcmSettings) const;
};

#endif //_GSMFRDEC_PCMPORT_H_
