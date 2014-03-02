/*****************************************************************************/
/**
*  © ST-Ericsson, 2009 - All rights reserved

*  Reproduction and Communication of this document is strictly prohibited
*  unless specifically authorized in writing by ST-Ericsson
*
* \brief   AMR Decoder class
* \author  ST-Ericsson
*/
/*****************************************************************************/
#ifndef _AMR_DECODER_H_
#define _AMR_DECODER_H_

#include "AFM_Decoder.h"

#ifndef HOST_ONLY
#include "AMRDecAlgo.h"
#else
#include "AMRHostDecAlgo.h"
#endif

/// @defgroup amrdec AMR Decoder
/// @ingroup afmdec
/// @{

OMX_ERRORTYPE amrhostdecFactoryMethod(ENS_Component_p *ppENSComponent);

#ifndef HOST_ONLY
OMX_ERRORTYPE amrdecFactoryMethod(ENS_Component_p * ppENSComponent);
#endif

class AMRDecoder_RDB : public ENS_ResourcesDB {
public:
	AMRDecoder_RDB(OMX_U32 nbOfDomains);
};

class AMRDecoder: public AFM_Decoder {
public:

	/// Constructor
	OMX_ERRORTYPE  construct(void);
    virtual ~AMRDecoder(void) {};

    virtual OMX_ERRORTYPE   createResourcesDB(void);
 
    AMRDecoder(bool isHost) : mIsHost(isHost) { };

	virtual int getSampleFrameSize(void) const {return 160;}
	virtual int getMaxChannels(void) const {return 1;}
	virtual int getMaxFrameSize(void) const {return 2560;}
	virtual int getSampleBitSize(void) const {return 16;}

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

    OMX_ERRORTYPE createAMRDecBitstreamPort(const OMX_AUDIO_PARAM_AMRTYPE &defaultAmrSettings);
	bool            mIsHost;
};

/// @}

#endif // _AMR_DECODER_H_


