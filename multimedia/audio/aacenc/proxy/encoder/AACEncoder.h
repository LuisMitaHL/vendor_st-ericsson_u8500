/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */
#ifndef _AAC_ENCODER_H_
#define _AAC_ENCODER_H_

#include "AFM_Encoder.h"
#include "AACHostEncAlgo.h"


/// @defgroup aacdec AAC Encoder
/// @ingroup afmenc
/// @{

/// AAC encoder factory method used when registering the AAC encoder
/// component to the ENS Core


OMX_ERRORTYPE aachostencFactoryMethod(ENS_Component_p *ppENSComponent);

class AACEncoder_RDB: public ENS_ResourcesDB {
 public:
  AACEncoder_RDB(OMX_U32 nbOfDomains);
};

class AACEncoder: public AFM_Encoder {
 public:
        AACEncoder(bool isHost) : mIsHost(isHost) { };
	/// Constructor
	OMX_ERRORTYPE  construct();
	virtual ~AACEncoder(void) {};
	
	virtual OMX_ERRORTYPE   createResourcesDB(void);

#ifndef OSI_TEST
 	virtual int getSampleFrameSize(void) const {return 1024 ;} // to be checked
#else
 	virtual int getSampleFrameSize(void) const {return 2048 ;} // to be checked
#endif

	virtual int getChannels(void) const {return 2;}
	virtual int getMaxChannels(void) const {return 2;}
	virtual int getSampleBitSize(void) const {return 16;}
	virtual int getMaxFrameSize(void) const {return 24576;}    // to be check??
	
	virtual OMX_ERRORTYPE getExtensionIndex(OMX_STRING cParameterName,
            OMX_INDEXTYPE* pIndexType) const;
   
  virtual OMX_U32 getUidTopDictionnary(void);
   
   
private:
    OMX_ERRORTYPE createAACEncBitstreamPort(const OMX_AUDIO_PARAM_AACPROFILETYPE &defaultAacSettings);
    bool            mIsHost;

};

/// @}

#endif // _AAC_ENCODER_H_


