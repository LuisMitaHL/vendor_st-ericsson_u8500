/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _BINARYSPLITTER_PCMPORT_H_
#define _BINARYSPLITTER_PCMPORT_H_

#include "AFM_PcmPort.h"
#include "audio_chipset_api_index.h"
#include "AFM_Symbian_AudioExt.h"

class BinarySplitter_PcmPort: public AFM_PcmPort {
public:
    BinarySplitter_PcmPort(
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

    OMX_ERRORTYPE updateBinarySplitterSettings(OMX_INDEXTYPE nParamIndex,
            OMX_PTR pComponentParameterStructure);

    virtual OMX_AUDIO_PARAM_PCMMODETYPE &       getPcmSettings()
    {
        return mBinarySplitterSettings;
    }

    inline OMX_BOOL isSynchronized(void) const {
        return mIsSynchronized;
    }

private:
    OMX_AUDIO_PARAM_PCMMODETYPE     mBinarySplitterSettings;
    OMX_BOOL                        mIsSynchronized;

};

typedef BinarySplitter_PcmPort* BinarySplitter_PcmPort_p;

#endif // _BINARYSPLITTER_PCMPORT_H_
