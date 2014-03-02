/*****************************************************************************/
/**
*  (c) ST-Ericsson, 2009 - All rights reserved
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \file   SPLNmfMpc.h
* \brief  SPL NMF proxy component
* \author ST-Ericsson
*/
/*****************************************************************************/
#ifndef _SPL_NMF_MPC_H_
#define _SPL_NMF_MPC_H_

#include "AFMNmfMpc_PcmProcessing.h"

#include "host/spl/nmfil/effect/configure.hpp"


/// @defgroup spl
/// @ingroup  afmpcmprocessing
/// @{

// SPL factory method used when registering the SPL
// Derived from AFM_PcmProcessing
class SPLNmfMpc : public AFMNmfMpc_PcmProcessing
{
    public:
        SPLNmfMpc(ENS_Component &enscomp): AFMNmfMpc_PcmProcessing(enscomp) {mNmfEffectsLib = 0; mISetHeap = 0;}

        /// Constructor
        virtual OMX_ERRORTYPE construct(void);
        virtual OMX_ERRORTYPE destroy(void);

        virtual OMX_ERRORTYPE instantiateAlgo(void);
        virtual OMX_ERRORTYPE startAlgo(void);
        virtual OMX_ERRORTYPE stopAlgo(void);
        virtual OMX_ERRORTYPE configureAlgo(void);
        virtual OMX_ERRORTYPE deInstantiateAlgo(void);

        virtual OMX_ERRORTYPE applyConfig(OMX_INDEXTYPE nConfigIndex,OMX_PTR pComponentConfigStructure);

    private:
        //handle of the NMF component which provides the effect interface
        //Spl library
        t_cm_instance_handle        mNmfEffectsLib;
        Imalloc_setheap             mISetHeap;
        Ispl_nmfil_effect_configure mIconfigure;
        SPLimiterParams_t           mSPLParamsMpc;
        SPLimiterConfig_t           mSPLConfigMpc;

        void                        registerStubsAndSkels(void);
        void                        unregisterStubsAndSkels(void);
};
/// @}
#endif //  _SPL_NMF_MPC_H_
