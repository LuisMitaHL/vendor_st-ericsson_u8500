/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _GSMFR_DECODER_H_
#define _GSMFR_DECODER_H_

#include "AFM_Decoder.h"
#include "GSMFRHostDecAlgo.h"

/// @defgroup gsmfrdec GSMFR Decoder
/// @ingroup afmdec
/// @{

/// GSMFR decoder factory method used when registering the GSMFR decoder 
/// component to the ENS Core
OMX_ERRORTYPE gsmfrhostdecFactoryMethod(ENS_Component_p *ppENSComponent);

/// Concrete class implementing a GSMFR decoder component
/// Derived from AFM_Decoder.
class GSMFRDecoder: public AFM_Decoder {
public:
	/// Constructor
	OMX_ERRORTYPE  construct(void);
  virtual ~GSMFRDecoder(void) {};

  GSMFRDecoder(bool isHost) : mIsHost(isHost) { };

	virtual int getSampleFrameSize(void) const {
		return 160;
	}

	virtual int getMaxChannels(void) const {
		return 1;
	}
	
	virtual int getMaxFrameSize(void) const {return 304;}
	virtual int getSampleBitSize(void) const {return 16;}

    virtual OMX_ERRORTYPE getExtensionIndex(OMX_STRING cParameterName,
            OMX_INDEXTYPE* pIndexType) const;
   
    virtual OMX_U32 getUidTopDictionnary(void);
protected:
        OMX_ERRORTYPE   createPcmPort(
                OMX_U32 nIndex,
                OMX_DIRTYPE eDir,
                OMX_U32 nBufferSizeMin,
                const OMX_AUDIO_PARAM_PCMMODETYPE &defaultPcmSettings);
private:

    OMX_ERRORTYPE createGSMFRBitstreamPort(const OMX_AUDIO_PARAM_GSMFRTYPE &defaultGsmfrSettings);

	bool            mIsHost;
};

/// @}

#endif // _GSMFR_DECODER_H_
