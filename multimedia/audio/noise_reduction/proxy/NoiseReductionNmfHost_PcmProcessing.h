/*****************************************************************************/
/**
*  © ST-Ericsson, 2011 - All rights reserved

*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Noise Reduction Host nmf processing class header
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef _NOISE_REDUCTION_NMF_HOST_PCM_PROCESSING_H_
#define _NOISE_REDUCTION_NMF_HOST_PCM_PROCESSING_H_

#include "AFMNmfHost_PcmProcessing.h"
#include "noise_reduction/nmfil/host/effect/configure.hpp"
#include "noise_reduction/nmfil/host/effectWrapped.hpp"

/**
 * Class that manages the Noise Reduction NMF component for the host CPU.
 */
class NoiseReductionNmfHost_PcmProcessing: public AFMNmfHost_PcmProcessing
{

public:

    NoiseReductionNmfHost_PcmProcessing(ENS_Component &enscomp)
            : AFMNmfHost_PcmProcessing(enscomp) {}

    virtual OMX_ERRORTYPE construct(void);
    virtual OMX_ERRORTYPE destroy(void);

    /**
        * Apply config to the NMF component.
        *
        * Called as a consequence of a setConfig call to the component.
        */
    virtual OMX_ERRORTYPE applyConfig(
                    OMX_INDEXTYPE config_index,
                    OMX_PTR component_config_structure_p);

protected:

    virtual OMX_ERRORTYPE instantiateAlgo(void);
    virtual OMX_ERRORTYPE startAlgo(void);
    virtual OMX_ERRORTYPE stopAlgo(void);
    virtual OMX_ERRORTYPE configureAlgo(void);
    virtual OMX_ERRORTYPE deInstantiateAlgo(void);
    virtual OMX_U32       nbBitPerSampleProcessed() { return 16; }


private:
    void apply_settings();

    // ejohsan FIXME!
    Inoise_reduction_nmfil_host_effect_configure  mIConfig;
};

#endif // _NOISE_REDUCTION_NMF_HOST_PCM_PROCESSING_H_
