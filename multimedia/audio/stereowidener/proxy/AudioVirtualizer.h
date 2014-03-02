/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _AUDIO_VIRTUALIZER_H_
#define _AUDIO_VIRTUALIZER_H_


#include "AFM_PcmProcessing.h"
#include "AFM_MemoryPreset.h"
#include "AFM_Symbian_AudioExt.h"
#include "audio_chipset_api_index.h"
//#include "audio_render_chipset_api.h"

//// @defgroup audiovirtualizer
/// @ingroup  afmpcmprocessing
/// @{

/// Audio Virtualizer factory method used when registering the AudioVirtualizer
/// component to ENS Core
OMX_ERRORTYPE audiovirtualizerFactoryMethod(ENS_Component_p * ppENSComponent);

class AudioVirtualizer_RDB : public ENS_ResourcesDB {
public:
    AudioVirtualizer_RDB(OMX_U32 nbOfDomains);
};


/// Concrete class implementing a AudioVirtualizer component
/// Derived from AFM_PcmProcessing
class AudioVirtualizer: public AFM_PcmProcessing 
{
    public:
        /// Constructor
        OMX_ERRORTYPE  construct(void);
        
        virtual OMX_ERRORTYPE createResourcesDB(void);
        //virtual OMX_ERRORTYPE getExtensionIndex(OMX_STRING cParameterName,  OMX_INDEXTYPE *pIndexType) const;
        virtual OMX_ERRORTYPE setConfig             (OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure);
        virtual OMX_ERRORTYPE getConfig             (OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure) const;
              
        inline OMX_SYMBIAN_AUDIO_CONFIG_VIRTUALIZERTYPE * getOMX_Symbian_AudioVirtualizerConfig(void) const { 
            return (OMX_SYMBIAN_AUDIO_CONFIG_VIRTUALIZERTYPE *)&mOMX_Symbian_ConfigAudioVirtualizer; 
        }
        virtual OMX_U32 getUidTopDictionnary(void);
        virtual bool prioritiesSetForAllPorts() const {return false;};
    private:
        
        OMX_SYMBIAN_AUDIO_CONFIG_VIRTUALIZERTYPE mOMX_Symbian_ConfigAudioVirtualizer;
        //OMX_AUDIO_CONFIG_VIRTUALIZERTYPE        mAudioVirtualizerConfig;
};
/// @}
#endif //  _AUDIO_VIRTUALIZER_H_
