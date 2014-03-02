/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __EXIFMixer_ArmNmfProcessingComponent_H
#define __EXIFMixer_ArmNmfProcessingComponent_H


#include "VFM_Component.h"
#include "VFM_Port.h"
#include "VFM_NmfHost_ProcessingComponent.h"
#include <cm/inc/cm_macros.h>
#include "exif_mixer/arm_nmf/exifmixer.hpp"

//#include "EXIFMixer_Proxy.h"
#include "video_chipset_api_index.h"
#include "video_generic_chipset_api.h"

class EXIFMixer_Proxy;

/// @ingroup EXIFMixer
/// @brief Class that groups all the attributes (NMF components and interfaces)
/// used by the JPEG Encoder, and instantiation / deinstantiation support
/// for these NMF components.
/// @note this class derives from VFM_NMF
class EXIFMixer_ArmNmfProcessingComponent : public VFM_NmfHost_ProcessingComponent, public exif_mixer_arm_nmf_api_cmd_ackDescriptor
{
	friend class EXIFMixer_Proxy;
	private:
    virtual OMX_ERRORTYPE configure();
    Iexif_mixer_arm_nmf_api_setConfig setconfiguration;
    Iexif_mixer_arm_nmf_api_getConfig getconfiguration;
	EXIFMixer_Proxy *pProxyComponent;
    public:
    	EXIFMixer_ArmNmfProcessingComponent(ENS_Component &enscomp);
        virtual ~EXIFMixer_ArmNmfProcessingComponent();
		OMX_HANDLETYPE OMXHandle;
        OMX_BOOL isSetConfigDone;
		omxilosalservices::OmxILOsalMutex* mMutexPendingCommand;
		OMX_U32 mNbPendingCommands;
        OMX_CONFIG_GPSLOCATIONTYPE gpsLocationValues;
		//Ijpegenc_sw_arm_nmf_api_setParam	setparamitf;
        //Iexif_mixer_core_api_setConfiguration mISplitterNmfCoreConfig;
        //Iexif_mixer_coresplitter_core_api_setParameter     mISplitterNmfCoreParam;


	protected:
		 /// @brief Register the stubs and skels from ARM to DSP for the internal interfaces of the
		 /// NMF Composite that is instantiated. This may be empty for full-sw implementation
		 virtual void registerStubsAndSkels(void);

        //virtual OMX_ERRORTYPE allocateBuffer(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_U32 nSizeBytes, OMX_U8 **ppData, void **bufferAllocInfo, void **portPrivateInfo);
        //virtual OMX_ERRORTYPE useBuffer(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_BUFFERHEADERTYPE* pBufferHdr, void **portPrivateInfo);
        //virtual OMX_ERRORTYPE useBufferHeader(OMX_DIRTYPE dir, OMX_BUFFERHEADERTYPE* pBuffer);
        //virtual OMX_ERRORTYPE freeBuffer(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_BOOL bBufferAllocated, void *bufferAllocInfo, void *portPrivateInfo);

		 /// @brief Unregister the stubs and skels from ARM to DSP for the internal interfaces of the
		 /// NMF Composite that is instantiated. This may be empty for full-sw implementation
		 virtual void unregisterStubsAndSkels(void);

		 /// @brief Create the codec and initialize the member mCodec
		 virtual OMX_ERRORTYPE codecCreate(OMX_U32 domainId);

		 /// @brief Destroy the codec and initialize the member mCodec
		 virtual void codecDestroy(void);

		 /// @brief Instantiate the specific interfaces between the proxy and the ddep.
		 /// It includes the interface emptyThisBuffer /  FillThisBuffer, as well as specific
		 /// interfaces.
		 virtual OMX_ERRORTYPE codecInstantiate(void);

		 /// @brief Start the NMF component. This may be empty in the specific implementation,
		 /// apart if any specific actions has to be taken.
		 virtual OMX_ERRORTYPE codecStart(void);

		 /// @brief Stop the NMF component. This may be empty in the specific implementation,
		 /// apart if any specific actions has to be taken.
		 virtual OMX_ERRORTYPE codecStop(void);

		 /// @brief Configure the NMF component. This may be empty in the specific implementation,
		 /// apart if any specific actions has to be taken.
		 virtual OMX_ERRORTYPE codecConfigure(void);

		 /// @brief Deinstantiate the specific interfaces between the proxy and the ddep.
		 /// It includes the interface emptyThisBuffer /  FillThisBuffer, as well as specific
		 /// interfaces.
		 virtual OMX_ERRORTYPE codecDeInstantiate(void);

		 /// @todo to document
		 virtual void sendConfigToCodec(){};

//		 virtual OMX_ERRORTYPE emptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer)
//		 {return NmfHost_ProcessingComponent::emptyThisBuffer(pBuffer);}

//		 virtual OMX_ERRORTYPE fillThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer)
//		  {return NmfHost_ProcessingComponent::fillThisBuffer(pBuffer);}

		 virtual OMX_ERRORTYPE doSpecificEventHandler_cb(OMX_EVENTTYPE event, OMX_U32 nData1, OMX_U32 nData2, OMX_BOOL &deferEventHandler);
		 virtual OMX_ERRORTYPE doSpecificSendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData, OMX_BOOL &bDeferredCmd);
		 virtual OMX_ERRORTYPE emptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer);
		 virtual OMX_ERRORTYPE fillThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer);
		 virtual void doSpecificEmptyBufferDone_cb(OMX_BUFFERHEADERTYPE* pBuffer);
         virtual void doSpecificFillBufferDone_cb(OMX_BUFFERHEADERTYPE* pBuffer);
		 virtual void pendingCommandAck();
		 virtual OMX_ERRORTYPE applyConfig(
                   OMX_INDEXTYPE nParamIndex,
                   OMX_PTR pComponentConfigStructure);
		void addPendingCommand();
		virtual OMX_ERRORTYPE retrieveConfig(OMX_INDEXTYPE nConfigIndex,OMX_PTR pComponentConfigStructure);

	private:
		void exif_mixer_process_assert(OMX_U32 condition, OMX_U32 errorType, OMX_U32 line, OMX_BOOL isFatal);
};

#endif //__EXIFMixer_ArmNmfProcessingComponent_H
