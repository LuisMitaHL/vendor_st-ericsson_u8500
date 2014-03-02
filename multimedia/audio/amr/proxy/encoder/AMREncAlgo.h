/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved

*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   AMR Decoder Algo class headers
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef _NMF_AMR_ENC_ALGO_H_
#define _NMF_AMR_ENC_ALGO_H_

#include "ENS_Nmf.h"
#include "NmfComponent.h"

#include "host/amr/nmfil/encoder/configure.hpp"
#include "host/malloc/setheap.hpp"
#include "AFM_MemoryPreset.h"

class AMREncAlgo : public NmfComponent {
    public:
        virtual ~AMREncAlgo() {};


        virtual OMX_ERRORTYPE instantiate(OMX_U32 domainId,OMX_U32 priority);
        virtual OMX_ERRORTYPE deInstantiate();

        virtual OMX_ERRORTYPE start();
        virtual OMX_ERRORTYPE stop();

        virtual OMX_ERRORTYPE configure();

        OMX_ERRORTYPE setParameter(AmrEncParam_t amrNmfParams);
        OMX_ERRORTYPE setConfig(AmrEncConfig_t amrNmfConfig);

    private :
        Iamr_nmfil_encoder_configure    mIConfig;
        Imalloc_setheap                 mISetHeap;
        t_afm_memory_need       mMemoryNeed;
        t_cm_memory_handle      mHeap[MEM_BANK_COUNT];

        // singletons
        t_cm_instance_handle mNmfAmrEncLib;
        t_cm_instance_handle mNmfAmrEnc24Lib;
        t_cm_instance_handle mNmfAmrCommonLib;
        t_cm_instance_handle mNmfMalloc;
        t_cm_instance_handle mNmfBitstreamLib;

        OMX_ERRORTYPE instantiateAlgoLibraries(void);
        OMX_ERRORTYPE deInstantiateAlgoLibraries(void);
        OMX_ERRORTYPE startAlgoLibraries(void);
        OMX_ERRORTYPE stopAlgoLibraries(void);

};


#endif /* _NMF_AMR_ENC_ALGO_H_ */
