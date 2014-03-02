/*****************************************************************************/
/**
*  © ST-Ericsson, 2010 - All rights reserved
*
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   ALSA source NMF Host class header
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef _ALSASOURCE_NMF_HOST_HPP_
#define _ALSASOURCE_NMF_HOST_HPP_

#include "AFMNmfHost_ProcessingComp.h"
#include "alsasource/nmfil/host/wrapper/configure.hpp"
#include "alsasource/nmfil/host/alsasource/configure.hpp"
#include "alsasource/nmfil/host/wrapper/process.hpp"

#define FIFO_SIZE 10
#define ALSA_DEVICENAME_MAX 32

/**
 * @brief Class that manages the ALSA source NMF component for the host CPU.
 */
class AlsaSourceNmfHost: public AFMNmfHost_ProcessingComp {
public:

	AlsaSourceNmfHost(ENS_Component &enscomp);
	~AlsaSourceNmfHost();

	/**
	 * @brief Apply settings to the NMF component.
	 *
	 * Called as a consequence of a setConfig call to the component.
	*/
	OMX_ERRORTYPE applyConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure);

	virtual OMX_ERRORTYPE retrieveConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure);

protected:
    // handle of the NMF component which provides the alsasource interface
    NMF::Composite * mNmfil;

    OMX_ERRORTYPE instantiateMain(void);
    OMX_ERRORTYPE startMain(void);
    OMX_ERRORTYPE configureMain(void);
    OMX_ERRORTYPE stopMain(void);
    OMX_ERRORTYPE deInstantiateMain(void);

private:
	typedef enum {
		PORT_IDX_OUTPUT,
	} portIndex;

	Ialsasource_nmfil_host_wrapper_configure  mIMainConfig;
	Ialsasource_nmfil_host_alsasource_configure  mIAlgoConfig;
	Ialsasource_nmfil_host_wrapper_process mIProcess;
};

#endif // _ALSASOURCE_NMF_HOST_HPP_
