/*****************************************************************************/
/**
*  © ST-Ericsson, 2010 - All rights reserved

*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   ALSA sink proxy
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef _ALSASINK_HPP_
#define _ALSASINK_HPP_

#include "ENS_Component.h"
#include "AFM_Component.h"
#include "OMX_Types.h"
#include "audio_sourcesink_chipset_api.h"

/**
 * @brief Factory method for the AlsaSink ENS component
 *
 * This method creates a AlsaSink ENS component.
 */
OMX_ERRORTYPE alsasinkFactoryMethod(ENS_Component_p *ppENSComponent);

/**
 * @brief Class that implements the generic part of the Alsasink component
 */
class AlsaSink : public AFM_Component {
public:
	OMX_ERRORTYPE  construct(void);
   
	~AlsaSink() {}
  
	virtual OMX_ERRORTYPE getParameter(OMX_INDEXTYPE nParamIndex,  
					OMX_PTR pComponentParameterStructure) const;
  
	virtual OMX_ERRORTYPE setParameter(OMX_INDEXTYPE nParamIndex,  
					OMX_PTR pComponentParameterStructure);
  
	virtual OMX_ERRORTYPE setConfig(OMX_INDEXTYPE nIndex,  
					OMX_PTR pComponentConfigStructure);
  
	virtual OMX_ERRORTYPE getConfig(OMX_INDEXTYPE nIndex,  
					OMX_PTR pComponentConfigStructure) const;
    
	void getDeviceName(OMX_U8 *ALSADeviceName, int maxLength);
	OMX_AUDIO_ALSASINK_CT getCodingType(void);

#ifdef MMPROBE_ENABLED
	OMX_U32 getPcmProbeId(OMX_U32 portIdx);
	OMX_BOOL isPcmProbeEnabled(OMX_U32 portIdx);
#endif // MMPROBE_ENABLED

private:
	OMX_U8 mALSADeviceName[OMX_MAX_STRINGNAME_SIZE];
	OMX_AUDIO_ALSASINK_CT mCodingType;
#ifdef MMPROBE_ENABLED
	OMX_U32 mPcmProbeId[2];
	OMX_BOOL mPcmProbeEnabled[2];
#endif // MMPROBE_ENABLED
};

#endif // _ALSASINK_HPP_
