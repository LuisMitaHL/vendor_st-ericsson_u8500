/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef _AMRWB_ENCODER_H_
#define _AMRWB_ENCODER_H_

#include "AFM_Encoder.h"
#include "AMRWBHostEncAlgo.h"


/// @defgroup amrwbdec AMRWB Encoder
/// @ingroup afmenc
/// @{

/// AMRWB encoder factory method used when registering the AMRWB encoder
/// component to the ENS Core


OMX_ERRORTYPE amrwbhostencFactoryMethod(ENS_Component_p *ppENSComponent);

class AMRWBEncoder: public AFM_Encoder {
 public:
        AMRWBEncoder(bool isHost) : mIsHost(isHost) { };
	/// Constructor
	OMX_ERRORTYPE  construct();
    virtual ~AMRWBEncoder(void) {};

	virtual int getSampleFrameSize(void) const {return 320;}
	virtual int getChannels(void) const {return 1;}
	virtual int getMaxChannels(void) const {return 1;}
	virtual int getSampleBitSize(void) const {return 16;}
	virtual int getMaxFrameSize(void) const {return 560;}
    
    virtual OMX_ERRORTYPE getExtensionIndex(OMX_STRING cParameterName,
        OMX_INDEXTYPE* pIndexType) const;
    virtual OMX_ERRORTYPE setConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure);
    virtual OMX_ERRORTYPE getConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure) const;
    virtual OMX_U32 getUidTopDictionnary(void);
protected:
        OMX_ERRORTYPE   createPcmPort(
                OMX_U32 nIndex,
                OMX_DIRTYPE eDir,
                OMX_U32 nBufferSizeMin,
                const OMX_AUDIO_PARAM_PCMMODETYPE &defaultPcmSettings);
    
private:

    OMX_ERRORTYPE createAMRWBEncBitstreamPort(const OMX_AUDIO_PARAM_AMRTYPE &defaultAmrSettings);

	bool            mIsHost;

};

/// @}

#endif // _AMRWB_ENCODER_H_


