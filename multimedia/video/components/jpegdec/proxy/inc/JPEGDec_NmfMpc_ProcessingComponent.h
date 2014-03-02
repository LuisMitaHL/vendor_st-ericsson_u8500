/*
 * Copyright (C) ST-Ericsson SA 2010. All rights reserved.
 * This code is ST-Ericsson proprietary and confidential.
 * Any use of the code for whatever purpose is subject to
 * specific written permission of ST-Ericsson SA.
 */

#ifndef __JPEGDEC_PROCESSINGCOMP_H
#define __JPEGDEC_PROCESSINGCOMP_H

#include <stdio.h>
#include "VFM_NmfHost_ProcessingComponent.h"
#include "vfm_vdc_jpeg.idt"
#include "jpegdec/arm_nmf/jpegdeccomparm.hpp"
#include "jpegdec/arm_nmf/jpegdec_dual.hpp"

class JPEGDec_Proxy;

class JPEGDec_NmfMpc_ProcessingComponent: public VFM_NmfHost_ProcessingComponent,
							  public jpegdec_arm_nmf_api_portSettingsDescriptor
{
	virtual OMX_ERRORTYPE construct(void);

	virtual OMX_ERRORTYPE destroy(void);

	virtual	OMX_ERRORTYPE applyConfig(OMX_INDEXTYPE nParamIndex,OMX_PTR pComponentConfigStructure);

	virtual OMX_ERRORTYPE sendCommand(OMX_COMMANDTYPE eCmd, OMX_U32 nData);

	virtual OMX_ERRORTYPE emptyThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer);

	virtual OMX_ERRORTYPE fillThisBuffer(OMX_BUFFERHEADERTYPE* pBuffer);

	virtual OMX_ERRORTYPE allocateBuffer(OMX_U32 nPortIndex,
										 OMX_U32 nBufferIndex,
										 OMX_U32 nSizeBytes,
										 OMX_U8 **ppData,
										 void **bufferAllocInfo,
										 void **portPrivateInfo);

	virtual OMX_ERRORTYPE useBufferHeader(OMX_DIRTYPE dir,
										  OMX_BUFFERHEADERTYPE* pBuffer);

	virtual OMX_ERRORTYPE useBuffer(OMX_U32 nPortIndex,
									OMX_U32 nBufferIndex,
									OMX_BUFFERHEADERTYPE* pBufferHdr,
									void **portPrivateInfo);

	virtual OMX_ERRORTYPE freeBuffer(OMX_U32 nPortIndex,
									 OMX_U32 nBufferIndex,
									 OMX_BOOL bBufferAllocated,
									 void *bufferAllocInfo,
									 void *portPrivateInfo);

	virtual	OMX_ERRORTYPE retrieveConfig(OMX_INDEXTYPE nConfigIndex,
										 OMX_PTR pComponentConfigStructure) ;

	void jpegdec_nmfmpc_assert(OMX_ERRORTYPE omxError, OMX_U32 line, OMX_BOOL isFatal);

private:
	OMX_ERRORTYPE allocateRequiredMemory();

	/// @brief Prepare data to configure the algorithm
	///
	/// @todo clean this function with clean data structure
	OMX_ERRORTYPE configureAlgo(JPEGDec_Proxy *jpegdec);

	void processingInit();

	OMX_ERRORTYPE JPEG_Proxy_PortSettingsChangedNotify(OMX_PARAM_PORTDEFINITIONTYPE *portDef);

	OMX_ERRORTYPE updateOutputPortSettings(OMX_INDEXTYPE nParamIndex,
										   const OMX_PARAM_PORTDEFINITIONTYPE *portDef);

	OMX_ERRORTYPE detectPortSettingsAndNotify(OMX_PARAM_PORTDEFINITIONTYPE portDef);

public:
	JPEGDec_NmfMpc_ProcessingComponent(ENS_Component &enscomp);
	virtual ~JPEGDec_NmfMpc_ProcessingComponent();

	virtual void portSettings(void* portDef);
	void initiatePortSettingsChange(OMX_PARAM_PORTDEFINITIONTYPE *portDef);

	OMX_ERRORTYPE errorRecoveryDestroyAll(void);

protected:
	/// @brief Register the stubs and skels from ARM to DSP for the internal interfaces of the
	/// NMF Composite that is instantiated. This may be empty for full-sw implementation
	virtual void registerStubsAndSkels(void);

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
	virtual void sendConfigToCodec();


	virtual OMX_ERRORTYPE doSpecificEventHandle_cb(OMX_EVENTTYPE event, OMX_U32 nData1,
												   OMX_U32 nData2);

// Data Members
private:
	JPEGDec_Proxy *pProxyComponent;
	OMX_U8 memoryAllocated;

	Ijpegdec_arm_nmf_api_memoryRequirements iProvideMemory;
	Ijpegdec_arm_nmf_api_setConfig iSetConfig;
	Ijpegdec_arm_nmf_api_setParam IsetParamRef;
        Ijpegdec_arm_nmf_api_computeARMLoad icomputeARMLoad;

        Ijpegdec_arm_nmf_api_sendmemoryContext iSendContext;

	/// @brief Variable to specify channel ID allocated by Resource Manager
	OMX_U32		mChannelId;

	/*! Buffer descriptor for Debug buffer
	*/
	ts_ddep_buffer_descriptor		mDebugBufferDesc;

	/*! Memory Handle for Debug buffer
	*/
	//t_cm_memory_handle				mDebugBufferHandle;

	/*! Buffer descriptor for Param buffer
	*/
	ts_ddep_buffer_descriptor		mParamBufferDesc;

	/*! Memory Handle for Param buffer
	*/
	//t_cm_memory_handle				mParamBufferHandle;

	/*! Buffer descriptor for Param buffer
	*/
	ts_ddep_buffer_descriptor		mLinkListBufferDesc;

	/*! Memory Handle for Param buffer
	*/
	//t_cm_memory_handle				mLinkListParamBufferHandle;

	/*! Memory Handle for Line buffer : internal buffer required by FW
	*/
	//t_cm_memory_handle				mLineBufferHandle;

	/*! Buffer descriptor for Line buffer
	*/
	ts_ddep_buffer_descriptor		mLineBufferDesc;
    ts_ddep_buffer_descriptor		mHuffMemoryDesc;

	OMX_BOOL                        isMemoryAllocated;

	void* mpc_vfm_mem_ctxt;


	OMX_BOOL fullDVFSSet;


};

#endif //JPEGDEC_PROCESSINGCOMP_H
