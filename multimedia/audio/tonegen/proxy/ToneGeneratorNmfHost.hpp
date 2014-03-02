/*****************************************************************************/
/**
*  © ST-Ericsson, 2010 - All rights reserved
*
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Tone generator NMF Host class header
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef _TONEGENERATOR_NMF_HOST_HPP_
#define _TONEGENERATOR_NMF_HOST_HPP_

#include "AFMNmfHost_ProcessingComp.h"
#include "tonegen/nmfil/host/wrapper/configure.hpp"
#include "tonegen/nmfil/host/tonegen/configure.hpp"

/**
 * @brief Class that manages the Tone generator NMF component for the host CPU.
 */
class ToneGeneratorNmfHost: public AFMNmfHost_ProcessingComp {
public:

    ToneGeneratorNmfHost(ENS_Component &enscomp)
            : AFMNmfHost_ProcessingComp(enscomp) {}

    OMX_ERRORTYPE construct(void);

    /**
     * @brief Apply settings to the NMF component.
     *
     * Called as a consequence of a setConfig call to the component.
     */
    OMX_ERRORTYPE applyConfig(
            OMX_INDEXTYPE nConfigIndex,
            OMX_PTR pComponentConfigStructure);

protected:
    // handle of the NMF component which provides the algo interface
    NMF::Composite * mNmfil;

    OMX_ERRORTYPE instantiateMain(void);
    OMX_ERRORTYPE startMain(void);
    OMX_ERRORTYPE configureMain(void);
    OMX_ERRORTYPE stopMain(void);
    OMX_ERRORTYPE deInstantiateMain(void);

private:
    // virtual methods called for algo specific stuff
    OMX_ERRORTYPE instantiateAlgo();
    OMX_ERRORTYPE configureAlgo();
    OMX_ERRORTYPE deInstantiateAlgo();

    void applyToneGenerator();

    Itonegen_nmfil_host_wrapper_configure  mIMainConfig;
    Itonegen_nmfil_host_tonegen_configure  mIAlgoConfig;
};

#endif // _TONEGENERATOR_NMF_HOST_HPP_
