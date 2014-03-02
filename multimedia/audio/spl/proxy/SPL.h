/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file   SPL.h
* \brief  SPL proxy
* \author ST-Ericsson
*/
/*****************************************************************************/
#ifndef _SPL_H_
#define _SPL_H_


#include "AFM_PcmProcessing.h"
#include "AFM_MemoryPreset.h"
#include "audio_transducer_chipset_api.h"

/// @defgroup spl
/// @ingroup  afmpcmprocessing
/// @{

/// SPL factory method used when registering the SPL
/// component to ENS Core
OMX_ERRORTYPE splFactoryMethod(ENS_Component_p * ppENSComponent);

class SPL_RDB : public ENS_ResourcesDB {
public:
    SPL_RDB(OMX_U32 nbOfDomains);
};


/// Concrete class implementing a SPL component
/// Derived from AFM_PcmProcessing
class SPL: public AFM_PcmProcessing 
{
    public:
        SPL(void) {instance++;}
        ~SPL(void) {instance--;}

        /// Constructor
        OMX_ERRORTYPE  construct(void);
        
        virtual OMX_ERRORTYPE createResourcesDB(void);
        virtual OMX_ERRORTYPE setParameter (OMX_INDEXTYPE nParamIndex,  OMX_PTR pComponentParameterStructure);
        virtual OMX_ERRORTYPE getParameter (OMX_INDEXTYPE nParamIndex,  OMX_PTR pComponentParameterStructure) const;
        virtual OMX_ERRORTYPE setConfig    (OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure);
        virtual OMX_ERRORTYPE getConfig    (OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure) const;
        
        virtual OMX_ERRORTYPE getExtensionIndex(OMX_STRING cParameterName, OMX_INDEXTYPE* pIndexType) const;
        virtual bool prioritiesSetForAllPorts() const {return false;}
              
        inline AFM_AUDIO_PARAM_SPLTYPE *getSplParams(void) const
        { 
            return (AFM_AUDIO_PARAM_SPLTYPE *) &mOMXSPLParam; 
        }

        inline OMX_AUDIO_CONFIG_SPLLIMITTYPE *getSplConfig(void) const 
        { 
            return (OMX_AUDIO_CONFIG_SPLLIMITTYPE *) &mOMXSPLTypeConfig; 
        }

        
        inline OMX_AUDIO_CONFIG_VOLUMETYPE *getVolConfig(void) const
        { 
            return (OMX_AUDIO_CONFIG_VOLUMETYPE *) &mOMXSPLVolConfig; 
        }

        inline int getInstance(void) const
        { 
            return instance; 
        }
        
        virtual OMX_U32 getUidTopDictionnary(void);

    private:
        AFM_AUDIO_PARAM_SPLTYPE       mOMXSPLParam;
        OMX_AUDIO_CONFIG_SPLLIMITTYPE mOMXSPLTypeConfig;
        OMX_AUDIO_CONFIG_VOLUMETYPE   mOMXSPLVolConfig;

        static int                    instance;
};
/// @}
#endif //  _SPL_H_
