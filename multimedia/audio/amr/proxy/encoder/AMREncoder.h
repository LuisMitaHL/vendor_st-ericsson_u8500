/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved

*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   AMR Encoder class
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef _AMR_ENCODER_H_
#define _AMR_ENCODER_H_

#include "AFM_Encoder.h"
#ifndef HOST_ONLY
#include "AMREncAlgo.h"
#else
#include "AMRHostEncAlgo.h"
#endif


/// @defgroup amrdec AMR Encoder
/// @ingroup afmenc
/// @{

/// AMR encoder factory method used when registering the AMR encoder
/// component to the ENS Core


OMX_ERRORTYPE amrhostencFactoryMethod(ENS_Component_p *ppENSComponent);
#ifndef HOST_ONLY
OMX_ERRORTYPE amrencFactoryMethod(ENS_Component_p * ppENSComponent);
#endif

class AMREncoder_RDB: public ENS_ResourcesDB {
 public:
  AMREncoder_RDB(OMX_U32 nbOfDomains);
};

class AMREncoder: public AFM_Encoder {
 public:
        AMREncoder(bool isHost) : mIsHost(isHost) { };
	/// Constructor
	OMX_ERRORTYPE  construct();
    virtual ~AMREncoder(void) {};
	virtual OMX_ERRORTYPE   createResourcesDB(void);

	virtual int getSampleFrameSize(void) const {return 160;}
	virtual int getChannels(void) const {return 1;}
	virtual int getMaxChannels(void) const {return 1;}
	virtual int getSampleBitSize(void) const {return 16;}
	virtual int getMaxFrameSize(void) const {return 304;}

    virtual OMX_ERRORTYPE getExtensionIndex(OMX_STRING cParameterName,
        OMX_INDEXTYPE* pIndexType) const;

    virtual OMX_ERRORTYPE setConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure);
    virtual OMX_ERRORTYPE getConfig(OMX_INDEXTYPE nConfigIndex, OMX_PTR pComponentConfigStructure) const;
    virtual OMX_U32 getUidTopDictionnary(void);
protected:
    virtual OMX_ERRORTYPE   createPcmPort(
                OMX_U32 nIndex,
                OMX_DIRTYPE eDir,
                OMX_U32 nBufferSizeMin,
                const OMX_AUDIO_PARAM_PCMMODETYPE &defaultPcmSettings);
    

private:

    OMX_ERRORTYPE createAMREncBitstreamPort(const OMX_AUDIO_PARAM_AMRTYPE &defaultAmrSettings);

	bool            mIsHost;

};

/// @}

#endif // _AMR_ENCODER_H_


