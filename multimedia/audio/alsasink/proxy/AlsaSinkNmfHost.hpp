/*****************************************************************************/
/**
*  © ST-Ericsson, 2010 - All rights reserved
*
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   Alsasink NMF Host class header
* \author  ST-Ericsson
*/
/*****************************************************************************/

#ifndef _ALSASINK_NMF_HOST_HPP_
#define _ALSASINK_NMF_HOST_HPP_

#include "AFMNmfHost_ProcessingComp.h"
#include "alsasink/nmfil/host/wrapper/configure.hpp"
#include "alsasink/nmfil/host/alsasink/configure.hpp"
#include "alsasink/nmfil/host/wrapper/process.hpp"

#define FIFO_SIZE 10

/**
 * @brief Class that manages the ALSA sink NMF component for the host CPU.
 */
class AlsaSinkNmfHost : public AFMNmfHost_ProcessingComp {
public:

	AlsaSinkNmfHost(ENS_Component &enscomp);
	~AlsaSinkNmfHost();

	/**
	 * @brief Apply settings to the NMF component.
	 *
	 * Called as a consequence of a setConfig call to the component.
	*/
	OMX_ERRORTYPE applyConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure);

	virtual OMX_ERRORTYPE retrieveConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure);

protected:
	// handle of the NMF component which provides the alsasink interface
	OMX_ERRORTYPE instantiateMain(void);
	OMX_ERRORTYPE startMain(void);
	OMX_ERRORTYPE configureMain(void);
	OMX_ERRORTYPE stopMain(void);
	OMX_ERRORTYPE deInstantiateMain(void);

private:
	typedef enum {
		PORT_IDX_INPUT,
		PORT_IDX_FEEDBACK
	} portIndex;

	Ialsasink_nmfil_host_wrapper_configure  mIMainConfig;
	Ialsasink_nmfil_host_alsasink_configure  mIAlgoConfig;
	Ialsasink_nmfil_host_alsasink_configure  mIAlgoSyncConfig;
	Ialsasink_nmfil_host_wrapper_process  mIProcess;
};

#endif // _ALSASINK_NMF_HOST_HPP_
