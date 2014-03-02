/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved

*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   SRC pcm processor proxy
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef _SRC_PCM_PROCESSING_H_
#define _SRC_PCM_PROCESSING_H_

#include "AFM_PcmProcessing.h"

OMX_ERRORTYPE srcPcmProcFactoryMethod(ENS_Component_p *ppENSComponent);

class SRCPcmProcessing : public AFM_PcmProcessing {
public:

    /// Constructor
    OMX_ERRORTYPE  construct(void);
    virtual ~SRCPcmProcessing() {};

    // Block size is defined as #samples / channel
    // Block size 480 => 480 samples / channel == 10 ms @ 48 kHz
    //virtual int getBlockSize(void) const {return 480;}
    // Block size 80 => 80 samples / channel == 10 ms @ 8 kHz
    virtual OMX_U32 getBlockSize(void) const {return 48;}
    // Sample bit size is 16 for host
    virtual OMX_U32 getSampleBitSize(void) const {return 16;} 

private:
};

#endif // _SRC_PCM_PROCESSING_H_
