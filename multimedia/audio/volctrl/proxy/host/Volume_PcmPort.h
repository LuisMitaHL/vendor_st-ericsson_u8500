/*****************************************************************************/
/**
*  © ST-Ericsson, 2010 - All rights reserved
*
*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson.
*
* \brief   Volume Control PCM port definition
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef _VOLUME_PCM_PORT_H_
#define _VOLUME_PCM_PORT_H_

#include "AFM_PcmPort.h"

/**
 * \brief Class that implements specific port handling in the Volume Control OMX component
 */
class Volume_PcmPort : public AFM_PcmPort
{
   public:

       /**
        * \brief Base class constructor
        */
       Volume_PcmPort(OMX_U32 nIndex, OMX_U32 nBufferSize,
                      const OMX_AUDIO_PARAM_PCMMODETYPE &defaultPcmSettings,
                      ENS_Component &enscomponent);

       /**
        * \brief Check specific PCM settings in the effect.
        * checkPcmSettings overrides the implementation from the base class AFM_PcmPort.
        * It allows upmixing or downmixing in the OMX IL Component which it is not possible
        * in the base class for setting an i/o PCM input port.
        */
       virtual OMX_ERRORTYPE checkPcmSettings(const OMX_AUDIO_PARAM_PCMMODETYPE &pcmSettings) const ;
};

#endif /* _VOLUME_PCM_PORT_H_ */
