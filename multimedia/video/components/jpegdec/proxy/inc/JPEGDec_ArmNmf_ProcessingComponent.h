/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __JPEGDec_ArmNmfProcessingComp_H
#define __JPEGDec_ArmNmfProcessingComp_H


#include "VFM_Component.h"
#include "VFM_Port.h"
#include "vfm_vdc_jpeg.idt"
#include "VFM_NmfHost_ProcessingComponent.h"
#include "jpegdec/arm_nmf/jpegdec_sw.hpp"

/// @ingroup JPEGDecoder
/// @brief Class that groups all the attributes (NMF components and interfaces)
/// used by the JPEG Decoder, and instantiation / deinstantiation support
/// for these NMF components.
/// @note this class derives from VFM_NMF
class JPEGDec_ArmNmf_ProcessingComponent : public VFM_NmfHost_ProcessingComponent , public jpegdec_arm_nmf_api_portSettingsDescriptor
{
    public:
        JPEGDec_ArmNmf_ProcessingComponent(ENS_Component &enscomp);
        virtual ~JPEGDec_ArmNmf_ProcessingComponent();

        virtual OMX_ERRORTYPE useBuffer(
                OMX_U32 nPortIndex,
                OMX_U32 nBufferIndex,
                OMX_BUFFERHEADERTYPE* pBufferHdr,
                void **portPrivateInfo);

            //: VFM_NmfHost_ProcessingComponent(enscomp) {};

		OMX_ERRORTYPE jpegDecCreatePortInterface(VFM_Port *port, const char *nameInput, const char *nameCB);

		OMX_ERRORTYPE errorRecoveryDestroyAll(void);

    		protected:
			 /// @brief Register the stubs and skels from ARM to DSP for the internal interfaces of the
			 /// NMF Composite that is instantiated. This may be empty for full-sw implementation
			 virtual void registerStubsAndSkels(void);

			 virtual OMX_ERRORTYPE sendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData);

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

			 virtual OMX_ERRORTYPE allocateBuffer(OMX_U32 nPortIndex,OMX_U32 nBufferIndex,OMX_U32 nSizeBytes,OMX_U8 **ppData,void **bufferAllocInfo,void **portPrivateInfo);

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

			 virtual OMX_ERRORTYPE applyConfig(OMX_INDEXTYPE nParamIndex,OMX_PTR pComponentConfigStructure);

			 void jpegdec_nmfarm_assert(OMX_ERRORTYPE omxError, OMX_U32 line, OMX_BOOL isFatal);


			 /// @todo to document
			 virtual void sendConfigToCodec();
			 OMX_ERRORTYPE allocateRequiredMemory();

			 virtual OMX_ERRORTYPE emptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer)
			 {return NmfHost_ProcessingComponent::emptyThisBuffer(pBuffer);}

			 virtual OMX_ERRORTYPE fillThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer)
			  {return NmfHost_ProcessingComponent::fillThisBuffer(pBuffer);}

			 virtual OMX_ERRORTYPE doSpecificEventHandle_cb(OMX_EVENTTYPE event, OMX_U32 nData1, OMX_U32 nData2)
			 { return OMX_ErrorNone;}

    protected:
//		ts_ddep_buffer_descriptor  *pParamBufferDesc;
        JPEGDec_Proxy *pProxyComponent;
		// methods inherited from armnmf_eventhandlerDescriptor
		virtual void portSettings(void* portDef);
		Ijpegdec_arm_nmf_api_setParam IsetParamRef;
		Ijpegdec_arm_nmf_api_setConfig IsetConfigRef;
		Ijpegdec_arm_nmf_api_memoryRequirements iProvideMemory;
       //methods inherited from jpegdec_sw_arm_nmf_api_setParamDescriptor
		//virtual void setParam(void * portDef);

	/*! Buffer descriptor for Param buffer
	*/
	ts_ddep_buffer_descriptor		mParamBufferDesc;

	/*! Memory Handle for Param buffer
	*/
	//t_cm_memory_handle				mParamBufferHandle;


	/*! Memory Handle for Line buffer : internal buffer required by Algo
	*/
	//t_cm_memory_handle				mLineBufferHandle;

	/*! Buffer descriptor for Line buffer
	*/
	ts_ddep_buffer_descriptor		mLineBufferDesc;
    ts_ddep_buffer_descriptor		mHuffMemoryDesc;

	OMX_BOOL                        isMemoryAllocated;

	void* arm_vfm_mem_ctxt;
};

#endif
