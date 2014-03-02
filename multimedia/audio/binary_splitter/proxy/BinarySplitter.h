/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _BINARYSPLITTER_H_
#define _BINARYSPLITTER_H_

#include "AFM_Component.h"
#include "audio_chipset_api_index.h"
#include "audio_codecs_chipset_api.h"

#define NB_OUTPUT_PORTS  2
#define NB_INPUT_PORTS  1

#ifndef HOST_ONLY
OMX_ERRORTYPE splitterFactoryMethod(ENS_Component_p * ppENSComponent);
#endif

OMX_ERRORTYPE splitterhostFactoryMethod(ENS_Component_p * ppENSComponent);

/// Concrete class implementing a BinarySplitter component
/// Derived from AFM_Component
class BinarySplitter: public AFM_Component {
public:
    virtual ~BinarySplitter();

    BinarySplitter(bool isHost) : mIsHost(isHost), mBlockSize(0) {};

    OMX_ERRORTYPE  construct(void);

    virtual OMX_ERRORTYPE getParameter(OMX_INDEXTYPE nParamIndex,
                                       OMX_PTR pComponentParameterStructure) const;
    virtual OMX_ERRORTYPE setConfig(
        OMX_INDEXTYPE nConfigIndex,
        OMX_PTR pComponentConfigStructure);

    virtual OMX_ERRORTYPE getConfig(
        OMX_INDEXTYPE nConfigIndex,
        OMX_PTR pComponentConfigStructure) const;

    inline OMX_SYMBIAN_AUDIO_CONFIG_CHANNELSWITCHBOARDTYPE * getAudioSwitchboardConfig(OMX_U8 i) const {
        return (OMX_SYMBIAN_AUDIO_CONFIG_CHANNELSWITCHBOARDTYPE *)&mAudioSwitchboardConfig[i];
    }

    inline bool getOutConfig(OMX_U8 i) const {
        return mOutConfig[i];
    }

    virtual OMX_U32 getUidTopDictionnary(void);

    virtual bool prioritiesSetForAllPorts() const {return false;};

private:
    bool           mIsHost;
    bool           mOutConfig[NB_OUTPUT_PORTS];
    OMX_U32        mBlockSize;
    OMX_SYMBIAN_AUDIO_CONFIG_CHANNELSWITCHBOARDTYPE        mAudioSwitchboardConfig[NB_OUTPUT_PORTS];
};

#endif //BINARYSPLITTER_H_
