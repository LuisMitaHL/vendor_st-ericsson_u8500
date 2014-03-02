/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   MixerOutputPort.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _MIXEROUTPUTPORT_H_
#define _MIXEROUTPUTPORT_H_

#include "MixerPort.h"

class MixerOutputPort: public MixerPort {
    public:
        MixerOutputPort(ENS_Component &enscomp, OMX_BOOL isHost)
            : MixerPort(0, OMX_DirOutput, enscomp),
                mIsHost(isHost) {};

        virtual OMX_ERRORTYPE setParameter(
                OMX_INDEXTYPE nParamIndex,
                OMX_PTR pComponentParameterStructure);
    private:
        OMX_BOOL    mIsHost;
};

#endif // _MIXEROUTPUTPORT_H_
