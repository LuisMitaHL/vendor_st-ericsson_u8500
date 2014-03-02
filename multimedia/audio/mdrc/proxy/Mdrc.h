/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _MDRC_H_
#define _MDRC_H_


#include "AFM_PcmProcessing.h"
#include "AFM_MemoryPreset.h"
#include "audio_chipset_api_index.h"
#include "AFM_Symbian_AudioExt.h"
#include "OMX_Symbian_ExtensionNames_Ste.h"
#ifndef HOST_ONLY
#include "host/mdrc/nmfil/effect/configure.hpp"
#endif /* !HOST_ONLY */
#ifndef __SYMBIAN32__
#include "host/mdrc/nmfil/host/effect/configure.hpp"
#endif // !__SYMBIAN32__


#define SIZEOF_CONFIG_TUNING_DATA (sizeof(OMX_SYMBIAN_AUDIO_CONFIG_TUNINGDATATYPE) - 1 + sizeof(AFM_AUDIO_GLOBAL_CONFIG_MDRC))


//// @defgroup mdrc
/// @ingroup  afmpcmprocessing
/// @{

/// mdrc factory method used when registering the Mdrc
/// component to ENS Core
#ifndef HOST_ONLY
OMX_ERRORTYPE mdrcFactoryMethod(ENS_Component_p * ppENSComponent);
#endif /* !HOST_ONLY */
#ifndef __SYMBIAN32__
OMX_ERRORTYPE mdrc_host_FactoryMethod(ENS_Component_p * ppENSComponent);
#endif /* !__SYMBIAN32__ */


/// Concrete class implementing a Mdrc component
/// Derived from AFM_PcmProcessing
class Mdrc: public AFM_PcmProcessing
{
    public:
        /// Constructor
        Mdrc(bool isHost) {mIsHost = isHost; nb_bits_quantif = (isHost ? 32 : 24);};

        OMX_ERRORTYPE         construct               (void);

        OMX_ERRORTYPE         getExtensionIndex       (OMX_STRING    cParameterName, OMX_INDEXTYPE *pIndexType) const;
        virtual OMX_ERRORTYPE setParameter            (OMX_INDEXTYPE nParamIndex,    OMX_PTR       pComponentParameterStructure);
        virtual OMX_ERRORTYPE getParameter            (OMX_INDEXTYPE nParamIndex,    OMX_PTR       pComponentParameterStructure) const;
        virtual OMX_ERRORTYPE setConfig               (OMX_INDEXTYPE nConfigIndex,   OMX_PTR       pComponentConfigStructure);
        virtual OMX_ERRORTYPE getConfig               (OMX_INDEXTYPE nConfigIndex,   OMX_PTR       pComponentConfigStructure) const;
        virtual OMX_U32       getUidTopDictionnary    (void);

        virtual bool          prioritiesSetForAllPorts(void) const {return false;}

        OMX_ERRORTYPE         copyConfig(MdrcParams_t *p_MdrcParams, MdrcConfig_t *p_configMpc);

        inline AFM_AUDIO_PARAM_MDRC *getMdrcParam(void) const
        {
            return (AFM_AUDIO_PARAM_MDRC *) &mMdrcParam;
        }


    protected:
        OMX_ERRORTYPE createPcmPort(OMX_U32                           nIndex,
                                    OMX_DIRTYPE                       eDir,
                                    OMX_U32                           nBufferSizeMin,
                                    const OMX_AUDIO_PARAM_PCMMODETYPE &defaultPcmSettings);

    private:
        bool                              mIsHost;
        int                               nb_bits_quantif;
        AFM_AUDIO_PARAM_MDRC              mMdrcParam;
        int                               mTuningData[SIZEOF_CONFIG_TUNING_DATA / sizeof(int) + 1];
        AFM_AUDIO_GLOBAL_CONFIG_MDRC      *pMdrcGlobalConfig;
        OMX_SYMBIAN_AUDIO_CONFIG_MDRCTYPE mMdrcConfig[5];
};
/// @}
#endif //  _MDRC_H_
