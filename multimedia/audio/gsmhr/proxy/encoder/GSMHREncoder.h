/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _GSMHR_ENCODER_H_
#define _GSMHR_ENCODER_H_

#include "AFM_Encoder.h"
#include "GSMHRHostEncAlgo.h"


/// @defgroup gsmhrenc GSMHR Encoder
/// @ingroup afmenc
/// @{

/// GSMHR encoder factory method used when registering the GSMHR encoder
/// component to the ENS Core

OMX_ERRORTYPE gsmhrhostencFactoryMethod(ENS_Component_p *ppENSComponent);


class GSMHREncoder: public AFM_Encoder {
 public:
        GSMHREncoder(bool isHost) : mIsHost(isHost) { };
	/// Constructor
	OMX_ERRORTYPE  construct();
    virtual ~GSMHREncoder(void) {};

	virtual int getSampleFrameSize(void) const {return 160;}
	virtual int getChannels(void) const {return 1;}
	virtual int getMaxChannels(void) const {return 1;}
	virtual int getSampleBitSize(void) const {return 16;}
	virtual int getMaxFrameSize(void) const {return 352;}
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

    OMX_ERRORTYPE createGSMHRBitstreamPort(const OMX_AUDIO_PARAM_GSMHRTYPE &defaultGsmhrSettings);

	bool            mIsHost;

};

/// @}

#endif // _GSMHR_ENCODER_H_


