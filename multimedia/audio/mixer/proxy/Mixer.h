/*****************************************************************************/
/*
 * Copyright (C) ST-Ericsson SA 2009,2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 *
 */

/**
 * \file   Mixer.h
 * \brief
 * \author ST-Ericsson
 */
/*****************************************************************************/
#ifndef _MIXER_H_
#define _MIXER_H_



#include "AFM_Symbian_AudioExt.h"
#include "AFM_Component.h"

#define NB_INPUT_PORTS  8
#define MAX_NB_CHANNELS 6

#define OUTPUT_PORT_IDX 0
#define INPUT_PORT_IDX  1
#define MS_GRANULARITY  5
#define MS_HOST_GRANULARITY  20

typedef struct MIXER_CONFIG_PORTBASETYPE {
  OMX_U32 nSize;
  OMX_VERSIONTYPE nVersion;
  OMX_U32 nPortIndex;
} MIXER_CONFIG_PORTBASETYPE;

/// Mixer factory method used when registering the Mixer
/// component to the ENS Core
OMX_ERRORTYPE mixerFactoryMethod(ENS_Component_p * ppENSComponent);
OMX_ERRORTYPE mixerhostFactoryMethod(ENS_Component_p * ppENSComponent);

/// Concrete class implementing a Mixer component
/// Derived from AFM_Component
class Mixer: public AFM_Component {
    public:
        Mixer(OMX_BOOL isHost):mIsHost(isHost) { };
        /// Constructor
        OMX_ERRORTYPE  construct(void);
        virtual ~Mixer(void);

        virtual OMX_ERRORTYPE getParameter(
                OMX_INDEXTYPE nParamIndex,
                OMX_PTR pComponentParameterStructure) const;

        virtual OMX_ERRORTYPE setConfig(
                OMX_INDEXTYPE nConfigIndex,
                OMX_PTR pComponentConfigStructure);

        virtual OMX_ERRORTYPE getConfig(
                    OMX_INDEXTYPE nConfigIndex,
                    OMX_PTR pComponentConfigStructure) const;

        virtual OMX_ERRORTYPE getExtensionIndex(
                OMX_STRING cParameterName,
                OMX_INDEXTYPE* pIndexType) const;

        virtual OMX_U32 getUidTopDictionnary(void);

        virtual OMX_BOOL runsOnHost() const { return mIsHost; };

        virtual bool prioritiesSetForAllPorts() const {return false;};
        virtual OMX_BOOL isPortSpecificParameter(OMX_INDEXTYPE nParamIndex) const;

private:
        OMX_BOOL    mIsHost;
};

#endif //_MIXER_H_
