/*****************************************************************************/
/**
*  © ST-Ericsson, 2010 - All rights reserved

*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   ALSA source proxy
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef _ALSASOURCE_HPP_
#define _ALSASOURCE_HPP_

#include "ENS_Component.h"
#include "AFM_Component.h"
#include "OMX_Types.h"
#include "audio_sourcesink_chipset_api.h"

/**
 * @brief Factory method for the ALSA source ENS component
 *
 * This method creates a AlsaSource ENS component.
 * It is used by the ENS Core to create a AlsaSource OMX IL component.
 * Register this method to ENS Core will enable ALSA source OMX IL support.
 */
OMX_ERRORTYPE alsasourceFactoryMethod(ENS_Component_p *ppENSComponent);

/**
 * @brief Class that implements the generic part of the Alsasource component
 */
class AlsaSource : public AFM_Component {
public:
	OMX_ERRORTYPE  construct(void);

	~AlsaSource() {}

	virtual OMX_ERRORTYPE getParameter(OMX_INDEXTYPE nParamIndex,
					OMX_PTR pComponentParameterStructure) const;

	virtual OMX_ERRORTYPE setConfig(OMX_INDEXTYPE nConfigIndex,
					OMX_PTR pComponentConfigStructure);

	virtual OMX_ERRORTYPE getConfig(OMX_INDEXTYPE nConfigIndex,
					OMX_PTR pComponentConfigStructure) const;

	OMX_U32 getBlockSize() const { return DEFAULT_PCM_BLOCKSIZE; }

	/** @brief Returns sample bit size. Overdrive only supports 16bit samples. */
	OMX_U32 getSampleBitSize(void) const { return 16; }

	void getDeviceName(OMX_U8 *ALSADeviceName, int maxLength);

#ifdef MMPROBE_ENABLED
	OMX_U32 getPcmProbeId(OMX_U32 portIdx);
	OMX_BOOL isPcmProbeEnabled(OMX_U32 portIdx);
#endif // MMPROBE_ENABLED

private:
	OMX_U8 mALSADeviceName[OMX_MAX_STRINGNAME_SIZE];
#ifdef MMPROBE_ENABLED
	OMX_U32 mPcmProbeId[1];
	OMX_BOOL mPcmProbeEnabled[1];
#endif // MMPROBE_ENABLED
};

#endif // _ALSASOURCE_HPP_
