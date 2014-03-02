/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _AMRWB_DECODER_H_
#define _AMRWB_DECODER_H_

#include "AFM_Decoder.h"

#include "AMRWBHostDecAlgo.h"

/// @defgroup amrwbdec AMRWB Decoder
/// @ingroup afmdec
/// @{

OMX_ERRORTYPE amrwbhostdecFactoryMethod(ENS_Component_p *ppENSComponent);


class AMRWBDecoder: public AFM_Decoder {
public:

	/// Constructor
	OMX_ERRORTYPE  construct(void);
        virtual ~AMRWBDecoder(void) {};

 
        AMRWBDecoder(bool isHost) : mIsHost(isHost) { };

	virtual int getSampleFrameSize(void) const {return 320;}
	virtual int getMaxChannels(void) const {return 1;}
	virtual int getMaxFrameSize(void) const {return 560;}
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

    OMX_ERRORTYPE createAMRWBDecBitstreamPort(const OMX_AUDIO_PARAM_AMRTYPE &defaultAmrSettings);
	bool            mIsHost;
};

/// @}

#endif // _AMRWB_DECODER_H_


