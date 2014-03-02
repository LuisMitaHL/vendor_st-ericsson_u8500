/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __MPEG4ENC_NMF_H
#define __MPEG4ENC_NMF_H

#include "VFM_NmfMpc_ProcessingComponent.h"
#include "vfm_vec_mpeg4.idt"
#include "host/mpeg4enc/mpc/api/ddep.hpp"
#include "MPEG4Enc_ProcessingComponent.h"

#define STAB_ARRAY_SIZE 32

typedef enum
{
	NOT_WRITTEN,
	WRITTEN_ONCE,
	WRITTEN_TWICE
} t_vos_written_times;

// forward definition
class MPEG4Enc_Proxy;

/// @ingroup MPEG4Encoder
/// @brief Class that groups all the attributes (NMF components and interfaces)
/// used by the MPEG4 Encoder, and instantiation / deinstantiation support
/// for these NMF components. It also groups the processing to be performed before
/// parameters are passed to MPC
/// @note this class derives from VFM_NmfProcessingComponent
class MPEG4Enc_NmfMpc_ProcessingComponent : public VFM_NmfMpc_ProcessingComponent
{
	friend class MPEG4Enc_Proxy;
	friend class MPEG4Enc_ProcessingComponent;

public:
	MPEG4Enc_NmfMpc_ProcessingComponent(ENS_Component &enscomp);
	virtual ~MPEG4Enc_NmfMpc_ProcessingComponent();

private:
	MPEG4Enc_Proxy *pProxyComponent;
	MPEG4Enc_ProcessingComponent processingComp;
	
	/// ----------------------------------------------
	/// Members to describe the NMF component and interfaces
	/// ----------------------------------------------

	/// @brief Variable to specify channel ID allocated by Resource Manager
	OMX_U32                                                mChannelId;

	/// @brief Handle to the Data Dependency NMF Component
	t_cm_instance_handle                                   ddHandle;

	/// @brief Handle to the BRC NMF Component
	t_cm_instance_handle                                   brcHandle;

	/// @brief Handle to the Mpeg4 Encoder Algo NMF Component
	t_cm_instance_handle                                   mNmfAlgoHandle;

	/// @brief Handle to the Resource Manager NMF Component
	t_cm_instance_handle                                   mNmfRmHandle;

	/// @brief NMF Interface from HOST to DD
	Impeg4enc_mpc_api_ddep                         iDdep;


	/// @brief Prepare data to configure buffer addresses that are passed to FW
	///
	/// @brief Prepare buffer addresses to be sent, through configureAlgo, to the firmware
	/// @param [in,out] mpeg4enc OpenMax Component related with this processing
	/// @param [out] pMecIn_parameters structure to be filled
	/// @todo clean this function with clean data structure

	void set_pMecOther_parameters(ts_ddep_vec_mpeg4_param_desc *ps_ddep_vec_mpeg4_param_desc, t_uint32 searchwinsize, t_uint32 mvfieldbuffersize, t_uint8 *headerBufferPhysicalAddress, t_uint8* fwdRefBufferPhysicalAddress, t_uint8* destBufferPhysicalAddress, t_uint8* intraRefBufferPhysicalAddress_in,t_uint8* intraRefBufferPhysicalAddress_out,t_uint8* motionVectorBufferPhysicalAddress, t_uint8* searchWindowBufferPhysicalAddress, t_uint8* mvFieldBufferPhyscialAddress);

	/*! Buffer descriptor for Debug buffer

	*/
	ts_ddep_buffer_descriptor                       mDebugBufferDesc;

	/*! Memory Handle for Debug buffer

	*/
	//< t_cm_memory_handle                              mDebugBufferHandle;

	/*! Buffer descriptor for Param buffer

	*/
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

	/// @brief Handle to the osttrace NMF Component
    t_cm_instance_handle                                   cOSTTrace;

    void* vfm_mem_ctxt;

	//< OMX_U32 internal_buffer_PoolId;

	t_vos_written_times vos_written;

private:
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
	virtual OMX_ERRORTYPE instantiate();

	/// @brief Start the NMF Components used by the Firmware
	/// @return OMX_ErrorNone if no error occured, an error otherwise
	virtual OMX_ERRORTYPE start();

	/// @brief Configure the NMF components used by the Firmware
	/// @return OMX_ErrorNone if no error occured, an error otherwise
	virtual OMX_ERRORTYPE configure();

	/// @brief Stop the NMF Components used by the MPEG4 Encode
	/// @return OMX_ErrorNone if no error occured, an error otherwise
	virtual OMX_ERRORTYPE stop();

	/// @brief Deinstantiates and unbinds the NMF Components used by the Firmware
	/// @return OMX_ErrorNone if no error occured, an error otherwise
	virtual OMX_ERRORTYPE deInstantiate();

	virtual OMX_ERRORTYPE doSpecificSendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData, OMX_BOOL& bDeferredCmd);
	virtual OMX_ERRORTYPE doSpecificEventHandler_cb(OMX_EVENTTYPE event, OMX_U32 nData1, OMX_U32 nData2, OMX_BOOL & deferEventHandler);
	
	virtual void doSpecificEmptyBufferDone_cb(OMX_BUFFERHEADERTYPE* pBuffer);
	virtual void doSpecificFillBufferDone_cb(OMX_BUFFERHEADERTYPE* pBuffer);

	virtual void registerStubsAndSkels(void);
	virtual void unregisterStubsAndSkels(void);

	virtual OMX_ERRORTYPE applyConfig(OMX_INDEXTYPE nParamIndex, OMX_PTR pComponentConfigStructure);

	virtual OMX_ERRORTYPE allocateBuffer(OMX_U32 nPortIndex, OMX_U32 nBufferIndex,	OMX_U32 nSizeBytes, OMX_U8 **ppData, void **bufferAllocInfo, void **portPrivateInfo);
	virtual OMX_ERRORTYPE useBuffer(OMX_U32 nPortIndex, OMX_U32 nBufferIndex, OMX_BUFFERHEADERTYPE* pBufferHdr, void **portPrivateInfo);

	virtual OMX_ERRORTYPE emptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer);

	OMX_ERRORTYPE errorRecoveryDestroyAll(void);


#ifdef __SYMBIAN32__
	OMX_BOOL fullDVFSSet;
#endif //__SYMBIAN32__

	OMX_ERRORTYPE configureAlgo();
};

#endif
