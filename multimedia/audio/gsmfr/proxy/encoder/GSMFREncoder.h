/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _GSMFR_ENCODER_H_
#define _GSMFR_ENCODER_H_

#include "AFM_Encoder.h"
#include "GSMFRHostEncAlgo.h"
/// @defgroup gsmfrenc GSMFR Encoder
/// @ingroup afmenc
/// @{

/// GSMFR encoder factory method used when registering the GSMFR encoder
/// component to the ENS Core

OMX_ERRORTYPE gsmfrhostencFactoryMethod(ENS_Component_p *ppENSComponent);


class GSMFREncoder: public AFM_Encoder {
public:
        GSMFREncoder(bool isHost) : mIsHost(isHost) { };
	/// Constructor
	OMX_ERRORTYPE  construct();
  virtual ~GSMFREncoder(void) {};
	
	virtual int getSampleFrameSize(void) const {
		return 160;
	}
	virtual int getChannels(void) const {
		return 1;
	}
	virtual int getMaxChannels(void) const {
		return 1;
	}
	virtual int getSampleBitSize(void) const {
		return 16;
	}
	virtual int getMaxFrameSize(void) const {
		return 304;
	}
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

#endif // _GSMFR_ENCODER_H_
