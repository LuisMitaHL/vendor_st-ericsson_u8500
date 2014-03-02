/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __MPEG4ENC_ArmNmf_ProcessingComponent_H
#define __MPEG4ENC_ArmNmf_ProcessingComponent_H

#include "VFM_NmfHost_ProcessingComponent.h"
#include "vfm_vec_mpeg4.idt"
//#include "host/mpeg4enc/mpc/api/ddep.hpp"
#include "MPEG4Enc_ProcessingComponent.h"

// forward definition
class MPEG4Enc_Proxy;

/// @ingroup MPEG4Encoder
/// @brief Class that groups all the attributes (NMF components and interfaces)
/// used by the MPEG4 Encoder, and instantiation / deinstantiation support
/// for these NMF components. It also groups the processing to be performed before
/// parameters are passed to ArmNmf
/// @note this class derives from VFM_NmfProcessingComponent
class MPEG4Enc_ArmNmf_ProcessingComponent : public VFM_NmfHost_ProcessingComponent
{
	friend class MPEG4Enc_Proxy;
	friend class MPEG4Enc_ProcessingComponent;
	
public:
	MPEG4Enc_ArmNmf_ProcessingComponent(ENS_Component &enscomp);
	virtual ~MPEG4Enc_ArmNmf_ProcessingComponent();

private:
	MPEG4Enc_Proxy *pProxyComponent;
	MPEG4Enc_ProcessingComponent processingComp;

	/// @brief Prepare data to configure buffer addresses that are passed to FW
	///
	/// @brief Prepare buffer addresses to be sent, through configureAlgo, to the firmware
	/// @param [in,out] mpeg4enc OpenMax Component related with this processing
	/// @param [out] pMecIn_parameters structure to be filled
	/// @todo clean this function with clean data structure

	void set_pMecOther_parameters(ts_ddep_vec_mpeg4_param_desc *ps_ddep_vec_mpeg4_param_desc, t_uint32 searchwinsize, t_uint32 mvfieldbuffersize, t_uint8 *headerBufferAddress, t_uint8* fwdRefBufferAddress, t_uint8* destBufferAddress, t_uint8* intraRefBufferAddress_in,t_uint8* intraRefBufferAddress_out,t_uint8* motionVectorBufferAddress, t_uint8* searchWindowBufferAddress, t_uint8* mvFieldBufferAddress);

	ts_ddep_buffer_descriptor                       mParamBufferDesc;

	/*! Memory Handle for Param buffer

	*/
	//< t_cm_memory_handle                              mParamBufferHandle;

	/*! Buffer descriptor for Param buffer

	*/
	ts_ddep_buffer_descriptor                       mInternalBufferDesc;

	/*! Memory Handle for Param buffer

	*/
	//< t_cm_memory_handle                              mInternalBufferHandle;

	/*! Buffer descriptor for Param buffer

	*/
	ts_ddep_buffer_descriptor                       mLinkListBufferDesc;

	/*! Memory Handle for Param buffer

	*/
	//< t_cm_memory_handle                              mLinkListBufferHandle;

	/*! Memory Handle for Internal buffer

	*/
	//< t_cm_system_address     mInternalBufferSysAddr;

    void* vfm_mem_ctxt;

	//< OMX_U32 internal_buffer_PoolId;

private:

	Impeg4enc_arm_nmf_api_setParam	setparamitf;
	Impeg4enc_arm_nmf_api_setCropForStab setcropstabitf;

	virtual OMX_ERRORTYPE allocateBuffer(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_U32 nSizeBytes, OMX_U8 **ppData, void **bufferAllocInfo, void **portPrivateInfo);

	/// ----------------------------------------------
	/// Functions for the instantiation and deinstantiation of NMF components
	/// They are called directly by the ENS, and then by the VFM
	/// (cf. VFM_NmfMpcProcessingComponent::start() for an example)
	/// Instantiation is permformed directly by the final proxy
	/// (cf. MPEG4Enc_Proxy::instantiateNMF()).
	/// ----------------------------------------------

	/// @brief Instantiate and binds the NMF Components used by the Firmware
	/// @return OMX_ErrorNone if no error occured, an error otherwise
	/// @param [in] component pointer to the MPEG4Enc component
	/// @param [in] in_size size of the input port
	/// @param [in] out_size size of the output port
	/// @param [in] eventHandler_cb the call back call on sendcommand() acknowledgment
	virtual OMX_ERRORTYPE codecInstantiate();

	/// @brief Start the NMF Components used by the Firmware
	/// @return OMX_ErrorNone if no error occured, an error otherwise
	virtual OMX_ERRORTYPE codecStart();

	/// @brief Configure the NMF components used by the Firmware
	/// @return OMX_ErrorNone if no error occured, an error otherwise
	virtual OMX_ERRORTYPE codecConfigure();

	/// @brief Stop the NMF Components used by the MPEG4 Encode
	/// @return OMX_ErrorNone if no error occured, an error otherwise
	virtual OMX_ERRORTYPE codecStop();

	/// @brief Deinstantiates and unbinds the NMF Components used by the Firmware
	/// @return OMX_ErrorNone if no error occured, an error otherwise
	virtual OMX_ERRORTYPE codecDeInstantiate();

	/// @brief Create the codec and initialize the member mCodec
	virtual OMX_ERRORTYPE codecCreate(OMX_U32 domainId);

	/// @brief Destroy the codec and initialize the member mCodec
	virtual void codecDestroy(void);

	/// @todo to document
	virtual void sendConfigToCodec(){};

	virtual OMX_ERRORTYPE doSpecificSendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData, OMX_BOOL& bDeferredCmd);
	virtual OMX_ERRORTYPE doSpecificEventHandler_cb(OMX_EVENTTYPE event, OMX_U32 nData1, OMX_U32 nData2, OMX_BOOL & deferEventHandler);
	
	virtual void doSpecificEmptyBufferDone_cb(OMX_BUFFERHEADERTYPE* pBuffer);
	virtual void doSpecificFillBufferDone_cb(OMX_BUFFERHEADERTYPE* pBuffer);

	virtual void registerStubsAndSkels(void);
	virtual void unregisterStubsAndSkels(void);


	virtual OMX_ERRORTYPE emptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer);
	virtual OMX_ERRORTYPE fillThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer);

	virtual OMX_ERRORTYPE applyConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentConfigStructure);

	/// @brief Configure the NMF components used by the Firmware
	/// @return OMX_ErrorNone if no error occured, an error otherwise
	virtual OMX_ERRORTYPE configure();

	OMX_ERRORTYPE configureAlgo();

	OMX_ERRORTYPE errorRecoveryDestroyAll(void);
};

#endif
